import os
import struct
import time

FFMPEG_APP = r"D:\Soft\ffmpeg\bin\ffmpeg.exe"
MKVMERGE_APP = r" D:\Soft\#RecoverySoft#\mkvtoolnix\mkvmerge.exe"
APP = FFMPEG_APP
FFMPEG_CMD_FORMAT = r'{ffmpeg} -f mjpeg -i "{in_file}" -vcodec copy "{out_file}"'
MKVMERGE_CMD_FORMAT = r'{mkvmerge} -o "{out_file}" "{in_file}"'
CMD_LINE_FORMAT = FFMPEG_CMD_FORMAT

file_type  = {'h264': b'H264', 'mjpeg': b'NUMJ'}

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

def run():

    input_file = r'F:\Root\DATA 3\20161226\C00017\C00017S00A20161226213405164.dat    '
    input_file.rstrip()
    avi_file = r"F:\out.avi"

    file_path ="F:\\Root\\DATA 3\\20161226\\C00017\\C00017S00A20161226234608231.dat"
    info = os.stat(file_path)
    print("file size: ", info.st_size)
    print('file type: ', FileType(file_path))

    with open(file_path, "rb") as file:

        header_fmt = '<H'

        file.seek(64)
        raw = file.read(12)

        signature,flags,val1,size = struct.unpack('<HHII', raw)
        print('signature :', signature)
        print('flags :', flags)
        print('size :', size)

        pass

    #print(CMD_LINE_FORMAT.format(ffmpeg=APP, in_file=input_file, out_file=avi_file))
    #os.system(CMD_LINE_FORMAT.format(ffmpeg=APP, in_file=input_file, out_file=avi_file))



    pass


