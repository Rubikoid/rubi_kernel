import struct
import os
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("-o", "--output", help="output file", required=True)
parser.add_argument("-i", "--input", help="input directory", required=True)
args = parser.parse_args()

O_FILE = args.output
I_DIR = args.input

if not os.path.exists(I_DIR):
    raise Exception(f"{I_DIR} not exists")

HEADER_MAGIC = 0xABCDABCD
FILE_MAGIC = 0xAABBCCDD
FS_FILE = 0x1
FS_DIRECTORY = 0x2

header = "<II"  # magic offset
file_head = "<IIIB"  # magic size name_size type
directory_base = "<I"  # ptr

final_nodes = {}
dirs = {}
for root, subdirs, files in os.walk(I_DIR):
    print(root, subdirs, files)
    # if len(files) > 1:
    # print("---WATAFACK---")
    # print(root, subdirs, files)
    # print("---WATAFACK---")
    for i in files:
        relpath = (os.path.relpath(os.path.join(root, i), I_DIR))
        if relpath.count('/') <= 1:
            final_nodes[relpath] = 0
    for i in subdirs:
        relpath = (os.path.relpath(os.path.join(root, i), I_DIR))
        if relpath.count('/') == 0:
            if "/" in dirs:
                dirs["/"][relpath] = 0
            else:
                dirs["/"] = {relpath: 0}

print(final_nodes)
print(dirs)


data = struct.pack(header, HEADER_MAGIC, 1337)

for i in final_nodes:
    final_nodes[i] = len(data)
    name = os.path.basename(i)
    rname = os.path.join(I_DIR, i)
    data += struct.pack(file_head, FILE_MAGIC, os.stat(rname).st_size, len(name), FS_FILE)
    data += name.encode()  # struct.pack(string_name_base.format(size=len(name)), list())
    data += open(rname, 'rb').read()  # f"lole".encode()  # struct.pack(string_name_base.format(size=4), list())

for i in dirs:
    if i == "/":
        for j in dirs[i]:
            dirs[i][j] = len(data)
            nodes = [final_nodes[k] for k in final_nodes if os.path.dirname(k) == j]
            name = os.path.basename(j)
            print(f"doing {j} {nodes}")
            data += struct.pack(file_head, FILE_MAGIC, len(nodes), len(name), FS_DIRECTORY)
            data += name.encode()
            data += b"".join(struct.pack(directory_base, k) for k in nodes)

dirs_def_offset = len(data)

data += struct.pack(file_head, FILE_MAGIC, len(dirs["/"]), len("/"), FS_DIRECTORY)
data += b"/"
data += b"".join(struct.pack(directory_base, dirs['/'][k]) for k in dirs['/'])

data = struct.pack(header, HEADER_MAGIC, dirs_def_offset) + data[8:]

print(final_nodes)
print(dirs)
print(dirs_def_offset)
print(data[dirs_def_offset:])

with open(O_FILE, 'wb') as f:
    f.write(data)
