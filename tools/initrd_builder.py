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

header = "<II"  # magic count
file_head = "<IIIB"  # magic size name_size type
string_name_base = "<{size}c"

final_nodes = {}
dirs = {}
for root, subdirs, files in os.walk(I_DIR):
    print(root, subdirs, files)
    # if len(files) > 1:
    # print("---WATAFACK---")
    # print(root, subdirs, files)
    # print("---WATAFACK---")
    for i in files:
        final_nodes[(os.path.relpath(os.path.join(root, i), I_DIR))] = 0
    for i in subdirs:
        dirs[] = (os.path.relpath(os.path.join(root, i), I_DIR))
print(final_nodes)
print(dirs)

exit()
data = struct.pack(header, HEADER_MAGIC, 1337)

for i in final_nodes:
    name = os.path.basename(i)
    data += struct.pack(file_head, FILE_MAGIC, 4, len(name), FS_FILE)
    data += struct.pack(string_name_base.format(size=len(name)), list(name.encode()))
    data += struct.pack(string_name_base.format(size=4), list(f"lole".encode()))

for i in dirs:
    pass
