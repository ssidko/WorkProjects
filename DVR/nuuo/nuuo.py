import os
import struct
import time
from collections import namedtuple


FFMPEG_APP = r"D:\Soft\ffmpeg\bin\ffmpeg.exe"
MKVMERGE_APP = r" D:\Soft\#RecoverySoft#\mkvtoolnix\mkvmerge.exe"
APP = FFMPEG_APP
FFMPEG_CMD_FORMAT = r'{ffmpeg} -f mjpeg -i "{in_file}" -vcodec copy "{out_file}"'
MKVMERGE_CMD_FORMAT = r'{mkvmerge} -o "{out_file}" "{in_file}"'
CMD_LINE_FORMAT = FFMPEG_CMD_FORMAT

file_type  = {'h264': b'H264', 'mjpeg': b'NUMJ'}

FRAME_HEADER_SIZE = 12

FrameHeader = namedtuple('FrameHeader', ['sign', 'flags', 'unk', 'size'])


def NormalizeDirPath(path):
    path = path.rstrip()
    if path[len(path) - 1] != "\\":
        path += "\\"
    return path


def FileType(path):
    with open(path, 'rb') as file:
        header = file.read(4)
        for t in file_type:
            if header == file_type[t]:
                return t
    return "unknown"


def ReadFrameHeader(file):
    buffer = file.read(FRAME_HEADER_SIZE)
    if len(buffer) == FRAME_HEADER_SIZE:
        return FrameHeader._make(struct.unpack('<HHII', buffer))


def IsValidFileStructure(path):
    try:
        type = FileType(path)
        if type != "unknown":
            with open(path, "rb") as file:
                # Skip file header
                file.seek(64)
                frame_header = ReadFrameHeader(file)
                while frame_header:
                    if frame_header.sign == 0x01ff:
                        #print('Frame at: ', str(file.tell() - FRAME_HEADER_SIZE))
                        file.seek(frame_header.size, 1)
                        frame_header = ReadFrameHeader(file)
                        continue
                    if frame_header.sign == 0xffff:
                        file.seek(-FRAME_HEADER_SIZE, 1)
                        footer = file.read(12)
                        if footer == b'\xFF\xFF\xFF\xFF\x01\x00\xEE\xFF\x10\x00\x00\x00':
                            #print('Footer at: ', str(file.tell() - FRAME_HEADER_SIZE))
                            return True
                    return False

    except BaseException as exc:
        print("Error with file: ", path)
        print(exc)

    return False

def ValidateAllFiles(path):
    for file_name in os.listdir(path):
        file_path = path + file_name
        if os.path.isdir(file_path):
            file_path = NormalizeDirPath(file_path)
            ValidateAllFiles(file_path)
        else:
            if IsValidFileStructure(file_path):
                pass
            else:
                info = os.stat(file_path)
                print("File: %s; Size: %d bytes; Type: %s; Integrity: CORRUPTED" % (file_path, info.st_size, FileType(file_path)))
            pass
    pass

def run(src_dir, dst_dir):

    if not os.path.exists(src_dir):
        print("Source directory (%s) not exist" % (src_dir))
        return

    if not os.path.exists(dst_dir):
        print('Destionation directory not exist')
        print('Create destionation directory: "%s"' % (dst_dir))
        os.mkdir(dst_dir)
        if os.path.exists(dst_dir):
            print('Destination directory created')
        else:
            print("Could not create destination directory")
            return

    src_dir = "E:\\41631"

    src_dir = NormalizeDirPath(src_dir)

    #ValidateAllFiles(src_dir)

    #print(CMD_LINE_FORMAT.format(ffmpeg=APP, in_file=input_file, out_file=avi_file))
    #os.system(CMD_LINE_FORMAT.format(ffmpeg=APP, in_file=input_file, out_file=avi_file))

    return
