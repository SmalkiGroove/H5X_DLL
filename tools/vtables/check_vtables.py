"""Checks the vtable structs of src/structs: every member is a 4-byte slot, so its offset is its index * 4.

Reports a slot whose "// 0xNN" comment or call_N placeholder name doesn't match its position (a missing or extra
placeholder shifts every later slot). Usage: python tools/vtables/check_vtables.py [structs folder]
"""
import glob
import os
import re
import sys

STRUCT_RE = re.compile(r"struct (\w+_vtable) \{([\s\S]*?)^\};", re.M)
PLACEHOLDER_RE = re.compile(r"int\* call_(\d+);")
OFFSET_RE = re.compile(r"//\s*(0x[0-9A-Fa-f]+)")


def check(path):
    errors = []
    text = open(path, encoding="utf-8", errors="replace").read()
    for match in STRUCT_RE.finditer(text):
        name, body = match.group(1), match.group(2)
        index = 0
        for line in body.split("\n"):
            code = line.split("//")[0].strip()
            if not code.endswith(";"):
                continue
            offset = index * 4
            placeholder = PLACEHOLDER_RE.match(code)
            if placeholder and int(placeholder.group(1)) != offset:
                errors.append(f"{path}: {name}: {code} is at offset {offset}")
            comment = OFFSET_RE.search(line)
            if comment and not placeholder and int(comment.group(1), 16) != offset:
                errors.append(f"{path}: {name}: {code} documented at {comment.group(1)} but at offset {hex(offset)}")
            index += 1
    return errors


def main():
    folder = sys.argv[1] if len(sys.argv) > 1 else os.path.join(os.path.dirname(__file__), "..", "..", "src", "structs")
    errors = [error for path in sorted(glob.glob(os.path.join(folder, "*.h"))) for error in check(path)]
    for error in errors:
        print(error)
    print(f"{len(errors)} mismatch(es)")
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())
