import os
import sys
import struct

PATH = r'F:\44552'
APP_PATH = r'D:\Soft\#MSoft#\RarRecovery\rarec.exe'
RAR_FILE_EXT = ".rar"
RAR_SIGNATURE = 0x21726152

def IsValidRarFile(file_path):
    with open(file_path, "rb") as file:
        tmp = file.read(4)
        signature = struct.unpack('<I', tmp)[0]
        if signature == RAR_SIGNATURE:
            return True
        else:
            return False
    return False

def RarRecoverInDir(dir_path):
    for file_name in os.listdir(dir_path):
        file_path = os.path.join(dir_path, file_name)
        if os.path.isdir(file_path):
            RarRecoverInDir(file_path)
        if not file_name.upper().endswith(RAR_FILE_EXT.upper()):
            continue

        try:
            if IsValidRarFile(file_path):
                print("File:", file_path, "is GOOD")
                continue
        except:
            continue

        src_file_path = file_path + ".bad"

        try:
            os.rename(file_path, src_file_path)
            print("File:", file_path, "is BAD. Marked as BAD_FILE.")
        except:
            print("Error renaming file.", "File:", file_path)
            continue

        out_file_path = file_path
        cmd_line = r'%s "%s" "%s"' % (APP_PATH, src_file_path, out_file_path)
        os.system(cmd_line)


if __name__ == "__main__":
    RarRecoverInDir(PATH)
    print("--== Finished ==--")