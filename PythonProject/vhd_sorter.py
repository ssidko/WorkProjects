import os
import sys
import struct
import datetime

VHD_HDR_DISK_SIZE = "DiskSize"
VHD_HDR_DISK_TYPE = "DiskType"
VHD_HDR_DISK_TIME_STAMP = "TimeStamp"
VHD_HDR_PARENT_TIME_STAMP = "ParentTimeStamp"

TIME_DIFF = 946684800   # January 1, 2000 12:00:00 AM

VHD_DISK_FIXED = "Fixed"
VHD_DISK_DYNAMIC = "Dynamic"
VHD_DISK_DIFFERENCING = "Differencing"

VhdDiskType = {2: VHD_DISK_FIXED, 3: VHD_DISK_DYNAMIC, 4: VHD_DISK_DIFFERENCING}


def VhdFile(file_path):
    with open(file_path, "rb") as file:
        signature = file.read(8)
        if signature == b'conectix':
            return True


def ReadVhdFielHeader(file_path:str) -> dict:
    header =  dict()
    if VhdFile(file_path):
        with open(file_path, "rb") as file:

            file.seek(24)
            raw = file.read(4)
            time_stamp = struct.unpack(">I", raw)[0] + TIME_DIFF
            time_value = datetime.datetime.fromtimestamp(float(time_stamp))
            # print(time_value.strftime('%Y-%m-%d %H:%M:%S'))
            header[VHD_HDR_DISK_TIME_STAMP] = time_value.strftime('%Y-%m-%d %H:%M:%S')

            file.seek(48)
            raw = file.read(8)
            disk_size = struct.unpack(">Q", raw)[0]
            header[VHD_HDR_DISK_SIZE] = disk_size

            file.seek(60)
            raw = file.read(4)
            disk_type = struct.unpack(">I", raw)[0]
            header[VHD_HDR_DISK_TYPE] = VhdDiskType[disk_type]

            if header[VHD_HDR_DISK_TYPE] == VHD_DISK_DIFFERENCING:
                file.seek(512+56)
                raw = file.read(4)
                time_stamp = struct.unpack(">I", raw)[0] + TIME_DIFF
                time_value = datetime.datetime.fromtimestamp(float(time_stamp))
                # print(time_value.strftime('%Y-%m-%d %H:%M:%S'))
                header[VHD_HDR_PARENT_TIME_STAMP] = time_value.strftime('%Y-%m-%d %H:%M:%S')
            else:
                header[VHD_HDR_PARENT_TIME_STAMP] = ""

    return header



def GetParent(file_path):
    with open(file_path, "rb") as file:
        if not VhdFile(file_path):
            return None
        file.seek(512)
        dynamic_sign = file.read(8)
        if dynamic_sign != b'cxsparse':
            return None
        file.seek(512 + 64, 0)
        parent_raw = file.read(512)
        parent = str(parent_raw, encoding="UTF-16-BE").lower()
        pos = parent.find(".vhd")
        if pos != -1:
            parent = parent[0:pos + len(".vhd")]
        if parent and len(parent):
            if parent.endswith(".vhd"):
                return parent


def VhdFileCorrectSize(file_path):
    with open(file_path, "r+b") as file:
        if not VhdFile(file_path):
            return False
        file.seek(0, os.SEEK_END)
        size = file.tell()
        new_file_size = size
        for i in range(1, 16):
            pos = size - i*512
            file.seek(pos)
            signature = file.read(8)
            if signature == b'conectix':
                new_file_size = pos + 512
                break
        file.truncate(new_file_size)
        return size != new_file_size


def GetLeafs(nodes: dict, key: str, leafs: list):
    if key in nodes:
        for child in nodes[key]:
            GetLeafs(nodes, child, leafs)
    else:
        if key not in leafs:
            leafs.append(key)


if __name__ == "__main__":

    vhd_dir = "F:\\43558\\Result\\NFS\\940156d5-765c-b1dc-8dfd-7b077363f15a"
    # vhd_dir = "E:\\Result\\NFS\\940156d5-765c-b1dc-8dfd-7b077363f15a"

    file_key = {}
    parent_key = {}

    # VhdFileCorrectSize("F:\\43558\\Result\\NFS\\940156d5-765c-b1dc-8dfd-7b077363f15a\\14fb9dd4-486b-44c6-9ff4-c77982a799b3.vhd")
    # header = ReadVhdFielHeader("F:\\43558\\test\\c9bbbc14-e172-4542-a007-131c404437fa.vhd")

    counter = 0
    for file_name in os.listdir(vhd_dir):
        file_name = file_name.lower()
        file_path = os.path.join(vhd_dir, file_name)
        if os.path.isdir(file_path):
            continue

        '''
        if (VhdFileCorrectSize(file_path)):
            print(file_name, "- size corrected")
        else:
            print(file_name, "- NOT CORRECTED")
        continue
        '''

        if not VhdFile(file_path):
            continue

        parent = GetParent(file_path)
        file_key[file_name] = parent

        if parent:
            if parent in parent_key:
                parent_key[parent].append(file_name)
            else:
                parent_key[parent] = [file_name]

    top_files = []
    for file in file_key:
        GetLeafs(parent_key, file, top_files)

    branches = []
    for file in top_files:
        branch = []
        next_file = file
        while next_file in file_key:
            branch.insert(0, (next_file, file_key[next_file]))
            next_file = file_key[next_file]
        branches.append(branch)

    with open("F:\\43558\\branches.txt", "w") as text_file:
        for i in range(len(branches)):
            branch = branches[i]
            file_name = branch[0][0]
            header = ReadVhdFielHeader(os.path.join(vhd_dir, file_name))
            size = float(header[VHD_HDR_DISK_SIZE])
            size = size / (1024*1024*1024)
            text_file.write("# %03d File: %s, Size: %.2f Gb, Time: %s\n" % (i, file_name, size, header[VHD_HDR_DISK_TIME_STAMP]))
            text_file.write("===============================================\n")
            for i in range(len(branch)):
                header = ReadVhdFielHeader(os.path.join(vhd_dir, branch[i][0]))
                if header[VHD_HDR_DISK_TYPE] == VHD_DISK_DIFFERENCING:
                    line = "[%02d] File = %s, Parent = %s, Time: %s\n" % (i, str(branch[i][0]), str(branch[i][1]), header[VHD_HDR_DISK_TIME_STAMP])
                else:
                    line = "[%02d] File = %s, parent = %s\n" % (i, str(branch[i][0]), str(branch[i][1]))
                text_file.write(line)
            text_file.write("\n\n")


    print("Finish.")

    pass
