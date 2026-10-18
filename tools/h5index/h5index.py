"""Read, check and update the DB object index (index.bin) of Heroes V.

    python tools/h5index/h5index.py stats   <index.bin>                    types and counts
    python tools/h5index/h5index.py list    <index.bin> <TypeName>         hrefs of one type
    python tools/h5index/h5index.py diff    <old.bin> <new.bin>            added / removed hrefs per type
    python tools/h5index/h5index.py missing <index.bin> <game_data>        mod objects not in the index
    python tools/h5index/h5index.py update  <index.bin> <game_data> [--out FILE] [--base toe] [--delta] [--links]
                                            [--links-under PREFIX]...
    python tools/h5index/h5index.py remove  <index.bin> <href>... [--out FILE]

<game_data> is the mod's source folder (heroes-expansion/game_data): every sub-folder is one pak
and its files are indexed by their path inside it. `update` adds the missing objects and writes
--out (default: <index.bin> itself). --base toe starts from the Tribes of the East index
(a2p1-data.pak) instead of <index.bin>. --delta writes only the mod objects (untested in game).
AdvMapObjectLink entries fill the editor's object palette and are picked by hand: `update` lists
the missing ones but adds them only with --links.
Type names come from types.xml (game_data/core/types.xml) and from the index itself.
Format notes: .cursor/rules/h5x-re-index-bin.mdc.
"""
import argparse, collections, os, re, struct, sys, zipfile
import xml.etree.ElementTree as ET

DATA = r'G:/__H5__/H5Xdev/data'
INDEX_CLASS = 0x3027D440            # factory type id of the index object (object table record)

# --- binary saver encoding (h5x-re-save-format.mdc) -------------------------

def varint(b, i):
    if b[i] & 1 == 0: return b[i] >> 1, i + 1
    return struct.unpack_from('<I', b, i)[0] >> 1, i + 4

def fields(b, s, e):
    out, i = [], s
    while i < e:
        tag = b[i]; n, j = varint(b, i + 1)
        if j + n > e: raise ValueError(f'field [{tag}] at {i:#x} overruns its parent')
        out.append((tag, j, j + n)); i = j + n
    return out

def u32(b, a): return struct.unpack_from('<I', b, a)[0]

def field(tag, data):
    n = len(data)
    head = bytes([tag, n << 1]) if n < 0x80 else bytes([tag]) + struct.pack('<I', (n << 1) | 1)
    return head + data

def fu32(tag, v): return field(tag, struct.pack('<I', v))

# --- index ------------------------------------------------------------------

BUCKETS = 389

class Index:
    def __init__(self, entries, root_id=0x07DD6EA0, buckets=BUCKETS):
        self.entries = entries          # {type id: [href, ...]} in file order
        self.root_id, self.buckets = root_id, buckets

    @classmethod
    def load(cls, data):
        b = data
        top = {t: (a, z) for t, a, z in fields(b, 0, len(b))}
        root_id = u32(b, fields(b, *top[1])[0][1])
        chunk = fields(b, *top[2])[0]                       # [1] object chunk: [0] id, [1] fields
        obj = {t: (a, z) for t, a, z in fields(b, chunk[1], chunk[2])}
        hm = fields(b, *fields(b, *obj[1])[0][1:])         # [2] hash_map<type id, vector<CDBPtr>>
        buckets = next(u32(b, a) for t, a, z in hm if t == 4)
        keys = [u32(b, a) for t, a, z in hm if t == 1]
        vals = [(a, z) for t, a, z in hm if t == 2]
        entries = {}
        for k, (a, z) in zip(keys, vals):
            items = []
            for t, p, q in fields(b, a, z):
                if t == 1:
                    s = fields(b, p, q)[0]                  # CDBPtr: [2] href
                    items.append(b[s[1]:s[2]].decode('latin1'))
            entries[k] = items
        return cls(entries, root_id, buckets)

    def save(self):
        keys = sorted(self.entries, key=lambda k: -(k % self.buckets))   # file order: buckets high to low
        order = {k: i for i, k in enumerate(self.entries)}
        keys.sort(key=lambda k: (-(k % self.buckets), order[k]))
        hm = fu32(3, len(keys)) + fu32(4, self.buckets)
        hm += b''.join(fu32(1, k) for k in keys)
        for k in keys:
            items = self.entries[k]
            v = fu32(2, len(items)) + b''.join(field(1, field(2, h.encode('latin1'))) for h in items)
            hm += field(2, v)
        obj = field(1, fu32(0, self.root_id) + field(1, field(2, hm)))
        return (fu32(4, 4) + field(1, fu32(2, self.root_id))
                + field(0, struct.pack('<IIB', INDEX_CLASS, self.root_id, 1))
                + field(2, obj) + field(5, b''))

    def hrefs(self):
        return {h.lower() for v in self.entries.values() for h in v}

# --- type names ---------------------------------------------------------------

def type_names(types_xml, index=None):
    """TypeName -> TypeID from types.xml, completed with the root tags found in the index."""
    names = {}
    if types_xml and os.path.exists(types_xml):
        r = ET.parse(types_xml).getroot()
        for it in r.find('SharedClasses'):
            if it.find('TypeID') is not None:
                names[it.find('TypeName').text] = int(it.find('TypeID').text) & 0xFFFFFFFF   # stored signed
    if index:
        votes = collections.defaultdict(collections.Counter)
        for k, items in index.entries.items():
            for h in items:
                m = re.search(r'#xpointer\((?:id\([^)]*\))?/([^)/]+)\)$', h)
                if m: votes[m.group(1)][k] += 1
        for name, c in votes.items():
            if name not in names and len(c) == 1: names[name] = next(iter(c))
    return names

# --- scanning XDB files ---------------------------------------------------------

ROOT = re.compile(rb'<([A-Za-z_][\w.]*)[\s/>]')
INLINE = re.compile(rb'<[A-Za-z_][\w.]*\s[^>]*?\bid="([^"]+)"[^>]*>\s*<([A-Za-z_][\w.]*)[\s/>]')

def scan_xdb(path, data):
    """(type name, href) of the root object and of every inline object with an id."""
    i = data.find(b'?>')
    m = ROOT.search(data, i + 2 if 0 <= i < 64 else 0)
    if not m: return []
    root = m.group(1).decode()
    out = [(root, f'/{path}#xpointer(/{root})')]
    for m2 in INLINE.finditer(data, m.end()):
        t = m2.group(2).decode()
        out.append((t, f'/{path}#xpointer(id({m2.group(1).decode()})/{t})'))
    return out

def scan_game_data(folder):
    out = []
    for pak in sorted(os.listdir(folder)):
        base = os.path.join(folder, pak)
        if not os.path.isdir(base): continue
        for d, _, files in os.walk(base):
            for f in files:
                if f.lower().endswith('.xdb'):
                    full = os.path.join(d, f)
                    rel = os.path.relpath(full, base).replace(os.sep, '/')
                    out += [(t, h, pak) for t, h in scan_xdb(rel, open(full, 'rb').read())]
    return out

PALETTE = {'AdvMapObjectLink'}      # editor palette entries: picked by hand, not every link file is meant to show

def missing(index, names, scanned, skip=PALETTE):
    have = index.hrefs(); add = collections.defaultdict(list); unknown = collections.Counter(); skipped = []
    for t, h, pak in scanned:
        if t not in names: unknown[t] += 1; continue
        if h.lower() in have: continue
        if t in skip: skipped.append(h); continue
        add[names[t]].append(h); have.add(h.lower())
    return add, unknown, skipped

# --- commands -------------------------------------------------------------------

def load(path): return Index.load(open(path, 'rb').read())

def default_types(path):
    t = os.path.join(os.path.dirname(os.path.abspath(path)), 'types.xml')
    return t if os.path.exists(t) else None

def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('cmd', choices=['stats', 'list', 'diff', 'missing', 'update', 'remove'])
    ap.add_argument('args', nargs='+')
    ap.add_argument('--types', help='types.xml (default: next to the index, else <game_data>/core/types.xml)')
    ap.add_argument('--out'); ap.add_argument('--base', choices=['toe'])
    ap.add_argument('--delta', action='store_true')
    ap.add_argument('--links', action='store_true', help='also add missing AdvMapObjectLink (editor palette) entries')
    ap.add_argument('--links-under', action='append', default=[], metavar='PREFIX',
                    help='add missing links whose href starts with PREFIX (e.g. /MapObjects/_(AdvMapObjectLink)/Heroes/)')
    a = ap.parse_args()
    idx = load(a.args[0])
    types = a.types or default_types(a.args[0])
    if not types and len(a.args) > 1: types = os.path.join(a.args[1], 'core', 'types.xml')
    names = type_names(types, idx); ids = {v: k for k, v in names.items()}
    if a.cmd == 'stats':
        for k, v in sorted(idx.entries.items(), key=lambda kv: -len(kv[1])):
            print(f'{k:08x} {ids.get(k, "?"):40} {len(v)}')
        print('types', len(idx.entries), 'hrefs', sum(map(len, idx.entries.values())))
    elif a.cmd == 'list':
        for h in idx.entries.get(names[a.args[1]], []): print(h)
    elif a.cmd == 'diff':
        new = load(a.args[1])
        for k in sorted(set(idx.entries) | set(new.entries)):
            o, n = set(idx.entries.get(k, [])), set(new.entries.get(k, []))
            if o != n: print(f'{ids.get(k, hex(k))}: +{len(n - o)} -{len(o - n)}'); [print('  +', h) for h in sorted(n - o)]; [print('  -', h) for h in sorted(o - n)]
    elif a.cmd in ('missing', 'update'):
        base = idx
        if a.base == 'toe':
            base = Index.load(zipfile.ZipFile(os.path.join(DATA, 'a2p1-data.pak')).read('index.bin'))
            base.root_id = idx.root_id
        scanned = scan_game_data(a.args[1])
        add, unknown, skipped = missing(Index({}) if a.delta else base, names, scanned, set() if a.links else PALETTE)
        prefixes = tuple(p.lower() for p in a.links_under)
        if prefixes:
            chosen = [h for h in skipped if h.lower().startswith(prefixes)]
            skipped = [h for h in skipped if h not in chosen]
            if chosen: add[names['AdvMapObjectLink']].extend(chosen)
        for k, hs in sorted(add.items(), key=lambda kv: ids.get(kv[0], '')):
            print(f'{ids.get(k, hex(k))}: {len(hs)}')
            if a.cmd == 'missing': [print('  ', h) for h in hs]
        if unknown: print('tags without a class type id (not indexed):', dict(unknown.most_common(10)))
        if skipped:
            print(f'not added, editor palette links (add with --links): {len(skipped)}')
            [print('  ', h) for h in skipped]
        if a.cmd == 'update':
            out = Index({}, idx.root_id, idx.buckets) if a.delta else base
            for k, hs in add.items(): out.entries.setdefault(k, []).extend(sorted(hs, key=str.lower))
            path = a.out or a.args[0]
            open(path, 'wb').write(out.save())
            print('wrote', path, 'types', len(out.entries), 'hrefs', sum(map(len, out.entries.values())))
    elif a.cmd == 'remove':
        drop = {h.lower() for h in a.args[1:]}; n = 0
        for k, v in idx.entries.items():
            kept = [h for h in v if h.lower() not in drop]; n += len(v) - len(kept); idx.entries[k] = kept
        path = a.out or a.args[0]
        open(path, 'wb').write(idx.save())
        print('removed', n, 'of', len(drop), 'hrefs, wrote', path)

if __name__ == '__main__':
    main()
