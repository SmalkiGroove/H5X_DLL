"""Read H5X adventure maps (.h5m) without the editor.

    python tools/h5m/h5m.py summary <map.h5m>             players, towns, object counts
    python tools/h5m/h5m.py terrain <map.h5m> [under]     tagged tree of GroundTerrain.bin / UndergroundTerrain.bin
    python tools/h5m/h5m.py regions <map.h5m> <out> [floor]
                                                         <out>.png region map (y up) + <out>.txt regions, content, gates

Footprints (blocked / active tiles) come from the Shared .xdb files in the game paks
(G:\\__H5__\\H5Xdev\\data\\*.pak, h5x-*.pak override data.pak / a2p1-data.pak).
Needs numpy and Pillow (regions only). Format notes: .cursor/rules/h5x-map-format.mdc.
"""
import collections, glob, math, os, re, struct, sys, zipfile
import xml.etree.ElementTree as ET

DATA = r'G:/__H5__/H5Xdev/data'

# --- archive ---------------------------------------------------------------

class H5M:
    def __init__(self, path):
        self.zip = zipfile.ZipFile(path)
        maps = [n for n in self.zip.namelist() if n.lower().endswith('/map.xdb')]
        self.dir = maps[0][:-len('map.xdb')]            # Maps/Multiplayer/<Name>/
        self.root = ET.fromstring(self.read('map.xdb'))
    def read(self, name):
        return self.zip.read(self.dir + name)
    def terrain(self, floor=0):
        tag = 'GroundTerrainFileName' if floor == 0 else 'UndergroundTerrainFileName'
        return Terrain(self.read(self.root.find(tag).get('href')))

# --- binary saver encoding (same as .sav, see h5x-re-save-format.mdc) --------

def varint(b, i):
    if b[i] & 1 == 0: return b[i] >> 1, i + 1
    return struct.unpack_from('<I', b, i)[0] >> 1, i + 4

def fields(b, s, e):
    out, i = [], s
    while i < e:
        tag = b[i]; n, j = varint(b, i + 1)
        if j + n > e: return None
        out.append((tag, j, j + n)); i = j + n
    return out

def u32(b, a): return struct.unpack_from('<I', b, a)[0]

def dump(b, s, e, d=0):
    """Print a tagged tree, collapsing runs of same-tag siblings."""
    f, rep = fields(b, s, e), 0
    for k, (tag, a, z) in enumerate(f):
        rep = rep + 1 if k and tag == f[k - 1][0] else 0
        if rep >= 2:
            if k == len(f) - 1 or f[k + 1][0] != tag: print('  ' * d + f'... {rep - 1} more [{tag}]')
            continue
        x = b[a:z]; printable = len(x) > 2 and all(32 <= c < 127 for c in x)
        sub = fields(b, a, z) if z - a > 4 and not printable else None
        if sub:
            print('  ' * d + f'[{tag}] @{a:#x} len {z - a} {{'); dump(b, a, z, d + 1)
        elif z - a == 4: print('  ' * d + f'[{tag}] u32 {u32(b, a)}')
        elif printable: print('  ' * d + f'[{tag}] "{x.decode()}"')
        else: print('  ' * d + f'[{tag}] {z - a} B {x[:12].hex()}')

class Terrain:
    def __init__(self, b):
        import numpy as np
        self.raw = b
        top = {t: (a, z) for t, a, z in fields(b, 0, len(b))}
        inner = {t: (a, z) for t, a, z in fields(b, *top[1])}
        body = {t: (a, z) for t, a, z in fields(b, *inner[1])}
        self.w, self.h = u32(b, body[2][0]), u32(b, body[3][0])
        def grid(a, z):
            f = {t: (p, q) for t, p, q in fields(b, a, z)}
            w, h = u32(b, f[1][0]), u32(b, f[2][0]); raw = b[f[3][0]:f[3][1]]
            return np.frombuffer(raw, np.float32 if len(raw) == 4 * w * h else np.uint8).reshape(h, w)
        self.layers = []                                 # (tile href, weights[y][x]) in paint order
        for t, a, z in fields(b, *body[4]):
            if t != 1: continue
            f = {t2: (p, q) for t2, p, q in fields(b, a, z)}
            s = fields(b, *f[3])[0]
            self.layers.append((b[s[1]:s[2]].decode(), grid(*f[2])))
        self.height = grid(*body[5])                     # f32 per vertex
        self.grids = {t: grid(*body[t]) for t in (7, 8, 10, 15) if t in body}

# --- footprints from the paks ---------------------------------------------------

class Paks:
    def __init__(self):
        self.idx, self.cache = {}, {}
        paks = sorted(glob.glob(os.path.join(DATA, '*.pak')),
                      key=lambda p: (not os.path.basename(p).startswith(('data', 'a2p1')), p))
        for p in paks:
            z = zipfile.ZipFile(p)
            for n in z.namelist(): self.idx[n.lower()] = (z, n)     # later paks override
    def footprint(self, href):
        path = href.split('#')[0].lstrip('/').lower()
        if path not in self.cache:
            e = self.idx.get(path); fp = None
            if e:
                r = ET.fromstring(e[0].read(e[1]))
                tiles = lambda tag: [(int(i.find('x').text), int(i.find('y').text)) for i in r.iterfind(tag + '/Item')]
                fp = {k: tiles(k) for k in ('blockedTiles', 'activeTiles', 'passableTiles', 'holeTiles')}
            self.cache[path] = fp
        return self.cache[path]

def rotate(dx, dy, rot):
    for _ in range(round(rot / (math.pi / 2)) % 4):     # nearest quarter turn, counter-clockwise, y up
        dx, dy = -dy, dx
    return dx, dy

# --- model ----------------------------------------------------------------------

def objects(m, floor=None):
    for it in m.root.find('objects'):
        o = it[0]
        if floor is not None and int(o.find('Floor').text) != floor: continue
        s = o.find('Shared')
        yield dict(tag=o.tag, el=o, id=it.get('id'), rot=float(o.find('Rot').text),
                   x=math.floor(float(o.find('Pos/x').text)), y=math.floor(float(o.find('Pos/y').text)),
                   href=(s.get('href') or '') if s is not None else '')

def short(o):
    e = o['el']; h = re.sub(r'\.\(.*?\)\.xdb|\.xdb', '', o['href'].split('#')[0].split('/')[-1])
    if o['tag'] == 'AdvMapMonster':
        st = [(h, e.find('Amount').text)] + [(i.find('Creature').text[9:], i.find('Amount').text) for i in e.find('AdditionalStacks')]
        return 'Monster ' + ' + '.join(f'{a} {c}' for c, a in st)
    if o['tag'] == 'AdvMapGarrison':
        return 'Garrison ' + h + ' ' + ', '.join(f"{i.find('Count').text} {i.find('Creature').text[9:]}" for i in e.find('armySlots'))
    if o['tag'] == 'AdvMapTown':
        return f"Town {e.find('PlayerID').text} {e.find('Specialization').get('href').split('/')[3]}"
    if o['tag'] == 'AdvMapHero': return 'Hero ' + e.find('PlayerID').text
    if o['tag'] == 'AdvMapBuilding' and e.find('GroupID') is not None and e.find('GroupID').text != '0':
        return f"Building {h} group {e.find('GroupID').text}"
    return o['tag'][6:] + ' ' + h

def summary(m):
    r = m.root
    print(m.dir, 'size', r.find('TileX').text, 'x', r.find('TileY').text, 'underground', r.find('HasUnderground').text)
    for i, p in enumerate(r.find('players'), 1):
        if p.find('ActivePlayer').text == 'true':
            print(f"  PLAYER_{i} team {p.find('Team').text} main town {p.find('MainTown').get('href')}")
    print(' ', dict(collections.Counter(o['tag'] for o in objects(m)).most_common()))
    for o in objects(m):
        if o['tag'] in ('AdvMapTown', 'AdvMapHero') or 'Monolith' in o['href'] or 'Portal' in o['href']:
            print(f"  ({o['x']},{o['y']}) floor {o['el'].find('Floor').text} {short(o)}")

def regions(m, out, floor=0):
    import numpy as np
    from PIL import Image, ImageDraw
    W, H = int(m.root.find('TileX').text), int(m.root.find('TileY').text)
    t = m.terrain(floor); paks = Paks()
    low = t.height[:H + 1, :W + 1] <= 0.01
    water = low[:-1, :-1] & low[1:, :-1] & low[:-1, 1:] & low[1:, 1:]
    water |= t.grids[10][1:2 * H:2, 1:2 * W:2] != 0     # water / shore mask sampled at tile centres
    blocked = np.zeros((H, W), bool); active = {}; objs = list(objects(m, floor))
    for o in objs:
        fp = (paks.footprint(o['href']) if o['href'] else None) or {'blockedTiles': [], 'activeTiles': [(0, 0)]}
        for k in ('blockedTiles', 'activeTiles'):
            for dx, dy in fp[k]:
                dx, dy = rotate(dx, dy, o['rot']); X, Y = o['x'] + dx, o['y'] + dy
                if 0 <= X < W and 0 <= Y < H:
                    if k == 'blockedTiles': blocked[Y, X] = True
                    else: active[(X, Y)] = o
    free = ~blocked & ~water
    gates = [o for o in objs if o['tag'] in ('AdvMapMonster', 'AdvMapGarrison')]
    guard = np.zeros((H, W), int)
    for i, o in enumerate(gates, 1):                     # monsters guard their 8 neighbours
        zone = [(0, 0)] if o['tag'] == 'AdvMapGarrison' else [(dx, dy) for dx in (-1, 0, 1) for dy in (-1, 0, 1)]
        for dx, dy in zone:
            X, Y = o['x'] + dx, o['y'] + dy
            if 0 <= X < W and 0 <= Y < H and free[Y, X]: guard[Y, X] = i
    walk = free & (guard == 0)
    for X, Y in active: walk[Y, X] = False
    lab = np.zeros((H, W), int); n = 0
    for y in range(H):
        for x in range(W):
            if walk[y, x] and not lab[y, x]:
                n += 1; lab[y, x] = n; st = [(x, y)]
                while st:
                    cx, cy = st.pop()
                    for dx in (-1, 0, 1):
                        for dy in (-1, 0, 1):
                            X, Y = cx + dx, cy + dy
                            if 0 <= X < W and 0 <= Y < H and walk[Y, X] and not lab[Y, X]:
                                lab[Y, X] = n; st.append((X, Y))
    size = np.bincount(lab.ravel(), minlength=n + 1)
    def touching(tiles):
        s = set()
        for X, Y in tiles:
            for dx in (-1, 0, 1):
                for dy in (-1, 0, 1):
                    xx, yy = X + dx, Y + dy
                    if 0 <= xx < W and 0 <= yy < H and size[lab[yy, xx]] >= 3 and lab[yy, xx]: s.add(int(lab[yy, xx]))
        return s
    content = collections.defaultdict(list)
    for o in objs:
        if o['tag'] in ('AdvMapStatic', 'AdvMapMonster', 'AdvMapGarrison'): continue
        tiles = [k for k, v in active.items() if v is o] or [(o['x'], o['y'])]
        for r in touching(tiles): content[r].append((tiles[0], short(o)))
    with open(out + '.txt', 'w') as f:
        for r in sorted(range(1, n + 1), key=lambda r: -size[r]):
            if size[r] < 3: continue
            ys, xs = np.nonzero(lab == r)
            f.write(f'\n## R{r} size {size[r]} x {xs.min()}..{xs.max()} y {ys.min()}..{ys.max()}\n')
            for (X, Y), s in sorted(content[r], key=lambda c: c[1]): f.write(f'   ({X},{Y}) {s}\n')
        f.write('\n# Gates (monster guard zone / garrison tile -> regions it touches)\n')
        for i, o in enumerate(gates, 1):
            rs = sorted(touching([(int(x), int(y)) for y, x in zip(*np.nonzero(guard == i))]))
            f.write(f"({o['x']},{o['y']}) {short(o)} -> {['R%d' % r for r in rs]}\n")
    rng = np.random.default_rng(7); pal = rng.integers(60, 240, (n + 1, 3)).astype(np.uint8); pal[0] = (40, 40, 40)
    img = pal[lab]; img[water] = (40, 70, 160); img[guard > 0] = (230, 30, 30)
    for (X, Y), o in active.items():
        if o['tag'] not in ('AdvMapMonster', 'AdvMapGarrison'): img[Y, X] = (255, 255, 255)
    for o in gates:
        if o['tag'] == 'AdvMapGarrison' and 0 <= o['x'] < W and 0 <= o['y'] < H: img[o['y'], o['x']] = (255, 150, 0)
    S = 6; im = Image.fromarray(img[::-1]).resize((W * S, H * S), Image.NEAREST); d = ImageDraw.Draw(im)
    for r in range(1, n + 1):
        if size[r] >= 20:
            ys, xs = np.nonzero(lab == r); d.text((xs.mean() * S - 6, (H - ys.mean()) * S - 5), f'R{r}', fill=(0, 0, 0))
    for i in range(0, W, 10):
        d.text((i * S + 1, 1), str(i), fill=(255, 255, 255)); d.text((1, (H - i) * S - 10), str(i), fill=(255, 255, 255))
    im.save(out + '.png')

if __name__ == '__main__':
    cmd, path = sys.argv[1], sys.argv[2]; m = H5M(path)
    if cmd == 'summary': summary(m)
    elif cmd == 'terrain':
        b = m.read(m.root.find('UndergroundTerrainFileName' if 'under' in sys.argv[3:] else 'GroundTerrainFileName').get('href'))
        dump(b, 0, len(b))
    elif cmd == 'regions': regions(m, sys.argv[3], int(sys.argv[4]) if len(sys.argv) > 4 else 0)
