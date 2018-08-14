import os
import zlib

#
# Extracting file from Veeam Backup file (*.vbk)
#

VBK_FILE = r'E:\data-srv\data-srv backup.vbk'
OUT_FILE = r'F:\42816\Windows Server 2008 R2-flat.vmdk'
START_SECTOR = 16438
END_SECTOR = 1040282287

SECTOR_SIZE = 512
MAX_BLOCK_SIZE = 1*1024*1024


def FindBlockBegin(vbk_file, start_sector):
    current_sector = start_sector
    while True:
        try:
            vbk_file.seek(current_sector * SECTOR_SIZE)
            sign = vbk_file.read(2)
            if sign == b'\x78\x01':
                vbk_file.seek(current_sector * SECTOR_SIZE)
                return current_sector
            else:
                current_sector += 1
        except:
            break
    return None


def FindBlockEnd(vbk_file, start_sector):
    return FindBlockBegin(vbk_file, start_sector)


def ReadBlock(file, sector):
    block = bytearray()
    file.seek(sector*SECTOR_SIZE)
    block.expand(file.read(SECTOR_SIZE))
    while True:
        if len(block) >= MAX_BLOCK_SIZE:
            return block
        tmp = file.read(SECTOR_SIZE)
        if tmp[:2] == b'\x78\x01':
            file.seek(file.tell() - SECTOR_SIZE)
            return block
        else:
            block.expand(tmp)


def ExtractFile(vbk_file_name, start_sector, out_file_name):
    with open(vbk_file_name, "rb") as vbk_file:
        with open(out_file_name, "wb+"):
            offset = 0
            while True:
                begin_block = FindBlockBegin(vbk_file, start_sector)
                if begin_block is None:
                    break
                end_block = FindBlockEnd(vbk_file, start_sector)
                if end_block is None:
                    break
                block_size = (end_block - begin_block)*SECTOR_SIZE
                vbk_file.seek(begin_block*SECTOR_SIZE)
                compr_data = vbk_file.read()


if __name__ == "__main__":

    # ExtractFile(VBK_FILE, START_SECTOR, OUT_FILE)
    print("Finished")
    pass