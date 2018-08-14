import os
import sys
import struct
import time
from collections import namedtuple

FFMPEG_APP = r"D:\Soft\ffmpeg\bin\ffmpeg.exe"
MKVMERGE_APP = r" D:\Soft\#RecoverySoft#\mkvtoolnix\mkvmerge.exe"
APP = FFMPEG_APP
H264 = "h264"
MJPEG = "mjpeg"
FFMPEG_CMD_FORMAT = r'{ffmpeg} -y -f {vformat} -i "{in_file}" -vcodec copy "{out_file}"'
MKVMERGE_CMD_FORMAT = r'{mkvmerge} -o "{out_file}" "{in_file}"'

FRAME_HEADER_SIZE = 12

FrameHeader = namedtuple('FrameHeader', ['sign', 'flags', 'unk', 'size'])
FramePtr = namedtuple('FramePtr', ['offset', 'unk'])
file_type = {'h264': b'H264', 'mjpeg': b'NUMJ'}


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
        if FileType(path) != "unknown":
            with open(path, "rb") as file:
                footer_offset = 0
                # Skip file header
                file.seek(64)
                frame_header = ReadFrameHeader(file)
                while frame_header:
                    if frame_header.sign == 0x01ff:
                        # print('Frame at: ', str(file.tell() - FRAME_HEADER_SIZE))
                        file.seek(frame_header.size, 1)
                        frame_header = ReadFrameHeader(file)
                        continue
                    if frame_header.sign == 0xffff:
                        file.seek(-FRAME_HEADER_SIZE, 1)
                        footer = file.read(FRAME_HEADER_SIZE)
                        if footer == b'\xFF\xFF\xFF\xFF\x01\x00\xEE\xFF\x10\x00\x00\x00':
                            # print('Footer at: ', str(file.tell() - FRAME_HEADER_SIZE))
                            footer_offset = file.tell() - len(footer)
                            break
                    return False
                #
                # Chack tail - table of frame pointers
                #
                file.seek(-4, os.SEEK_END)
                table_size = struct.unpack("<I", file.read(4))[0]
                file_size = os.stat(path).st_size
                if footer_offset == (file_size - (0x4C + table_size * 8 + 4)):
                    for i in range(table_size):
                        file.seek(footer_offset + 0x4C + i * 8)
                        frame_ptr = FramePtr._make(struct.unpack("<II", file.read(8)))
                        file.seek(frame_ptr.offset)
                        frame_header = ReadFrameHeader(file)
                        if not frame_header or frame_header.sign != 0x01FF:
                            return False
                    return True
    except BaseException as exc:
        print("Error with file: ", path)
        print(exc)
    return False


def SaveVideoData(src_file_path, dst_file_path):
    with open(src_file_path, "rb") as src_file:
        with open(dst_file_path, "wb") as dst_file:
            src_file.seek(64)
            frame_header = ReadFrameHeader(src_file)
            while frame_header:
                if frame_header.sign == 0x01ff:
                    frame = src_file.read(frame_header.size)
                    dst_file.write(frame)
                    frame_header = ReadFrameHeader(src_file)
                    continue
                return


def CheckAllFiles(path):
    for file_name in os.listdir(path):
        file_path = path + file_name
        if os.path.isdir(file_path):
            file_path = NormalizeDirPath(file_path)
            CheckAllFiles(file_path)
        else:
            if IsValidFileStructure(file_path):
                pass
            else:
                info = os.stat(file_path)
                print("File: %s; Size: %d bytes; Type: %s; Integrity: CORRUPTED" %
                      (file_path, info.st_size, FileType(file_path)))


def RecoverAllFiles(src_dir, out_dir, tmp_file_name):
    try:
        for file_name in os.listdir(src_dir):
            file_path = src_dir + file_name
            if os.path.isdir(file_path):
                dir_path = NormalizeDirPath(file_path)
                RecoverAllFiles(dir_path, out_dir, tmp_file_name)
            else:
                if len(file_name) != 31:
                    print("File:", file_path)
                    print("Invalid file name length (%d), must be 31" % len(file_name))
                    continue

                # C00001S00A20160128162034809.dat
                date = file_name[10:14] + "-" + file_name[14:16] + "-" + file_name[16:18]
                time = file_name[18:20] + "-" + file_name[20:22] + "-" + file_name[22:24] + "," + file_name[24:27]
                camera = file_name[1:6]

                if IsValidFileStructure(file_path):

                    # temp_file_path = out_dir + "temp.h264"
                    temp_file_path = out_dir + tmp_file_name
                    SaveVideoData(file_path, temp_file_path)

                    out_dir_path = "{dir}{date}\\C{cam}\\".format(dir=out_dir, date=date, cam=camera)
                    avi_file_path = "{dir}[{cam}]-[{date} {time}].avi".format(dir=out_dir_path, cam=camera, date=date, time=time)

                    if not os.path.exists(out_dir_path):
                        os.makedirs(out_dir_path)

                    video_format = FileType(file_path)
                    if video_format == 'h264':
                        os.system(FFMPEG_CMD_FORMAT.format(ffmpeg=APP, vformat=H264, in_file=temp_file_path, out_file=avi_file_path))
                    elif video_format == 'mjpeg':
                        os.system(FFMPEG_CMD_FORMAT.format(ffmpeg=APP, vformat=MJPEG, in_file=temp_file_path, out_file=avi_file_path))
                    else:
                        return

                else:
                    info = os.stat(file_path)
                    print("File: %s; Size: %d bytes; Type: %s; Integrity: CORRUPTED" %
                          (file_path, info.st_size, FileType(file_path)))

    except BaseException as exc:
        print(exc)


def run(src_dir, dst_dir):

    if not os.path.exists(src_dir):
        print("Source directory (%s) not exist" % src_dir)
        return

    if not os.path.exists(dst_dir):
        print('Destination directory not exist')
        print('Create destination directory: "%s"' % dst_dir)
        os.makedirs(dst_dir)
        if os.path.exists(dst_dir):
            print('Destination directory created')
        else:
            print("Couldn`t create destination directory")
            return

    src_dir = NormalizeDirPath(src_dir)
    dst_dir = NormalizeDirPath(dst_dir)
    tmp_file_name = os.path.basename(os.path.normpath(src_dir)) + ".h264"

    RecoverAllFiles(src_dir, dst_dir, tmp_file_name)

    return

if __name__ == '__main__':

    print(sys.argv)
    if len(sys.argv) == 3:
        dst_dir = NormalizeDirPath(sys.argv[1])
        src_dir = NormalizeDirPath(sys.argv[2])
        tmp_file_name = os.path.basename(os.path.normpath(src_dir)) + ".h264"

        print('Start for dir:', src_dir)
        print('destination:', dst_dir)
        print('temp file name:', tmp_file_name)

        run(src_dir, dst_dir)

        print("Finished with:", src_dir)
    else:
        print('Usage: <destination dir_path> <source dir_path>')









