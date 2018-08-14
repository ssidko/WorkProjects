import os
import olefile
from zipfile import ZipFile, BadZipFile

PATH = r'E:\44749\Name\$LostFiles\unsorted'

checkers = list()

def isMp3(file):
    file.seek(0)
    sign = file.read(3)
    if (sign == b'ID3') or (sign[:2] == b'\xFF\xFB'):
        return (True, ".mp3")
    else:
        return (False, "")

def isFlac(file):
    file.seek(0)
    sign = file.read(4)
    if (sign == b'fLaC'):
        return (True, ".flac")
    else:
        return (False, "")


def isDwgFile(file):
    file.seek(0)
    sign = file.read(4)
    if (sign == b'AC10'):
        return (True, ".dwg")
    else:
        return (False, "")


def isJpeg(file):
    file.seek(0)
    sign = file.read(4)
    if (sign == b'\xFF\xD8\xFF\xE1') or (sign == b'\xFF\xD8\xFF\xE0'):
        return (True, ".jpg")
    else:
        return (False, "")


def isTiff(file):
    file.seek(0)
    sign = file.read(4)
    if (sign == b'\x49\x49\x2A\x00') or (sign == b'\x4D\x4D\x00\x2A'):
        return (True, ".tiff")
    else:
        return (False, "")


def isMov(file):
    file.seek(4)
    sign = file.read(4)
    if (sign == b'ftyp'):
        return (True, ".mov")
    else:
        return (False, "")

def isEps(file):
    file.seek(0)
    sign = file.read(4)
    if (sign == b'\xC5\xD0\xD3\xC6'):
        return (True, ".eps")
    else:
        return (False, "")


def isPdf(file):
    file.seek(0)
    sign = file.read(4)
    if (sign == b'%PDF'):
        return (True, ".pdf")
    else:
        return (False, "")


def isRtf(file):
    file.seek(0)
    sign = file.read(5)
    if (sign == b'\x7B\x5C\x72\x74\x66'):
        return (True, ".rtf")
    else:
        return (False, "")


def isAiff(file):
    file.seek(8)
    sign = file.read(8)
    if (sign == b'AIFFCOMM'):
        return (True, ".aiff")
    else:
        return (False, "")


def isMkv(file):
    file.seek(24)
    sign = file.read(8)
    if (sign == b'matroska'):
        return (True, ".mkv")
    else:
        return (False, "")


def isAvi(file):
    file.seek(0)
    sign = file.read(4)
    if (sign == b'RIFF'):
        file.seek(8)
        sign = file.read(8)
        if (sign == b'AVI LIST'):
            return (True, ".avi")
    return (False, "")


def isMSOffice2013(file):
    result = (False, "")
    if olefile.isOleFile(file.name):
        ole = olefile.OleFileIO(file.name)
        if ole.exists('WordDocument'):
            result = (True, ".doc")
        elif ole.exists('Workbook'):
            result = (True, ".xls")
        elif ole.exists('PowerPoint Document'):
            result = (True, ".ppt")
        ole.close()
    return result


def isRar(file):
    file.seek(0)
    sign = file.read(4)
    if (sign == b'Rar!'):
        return (True, ".rar")
    else:
        return (False, "")


def isZip(file):
    try:
        with ZipFile(file.name, "r") as zip_file:
            info_list = zip_file.infolist()
            content = list()
            for info in info_list:
                content.append(info.filename.lower())
            content.sort()
            if 'word/document.xml' in content:
                return (True, ".docx")
            elif 'xl/workbook.xml' in content:
                return (True, ".xlsx")
            else:
                return (True, ".zip")
    except BadZipFile:
        pass
    return (False, "")


def isIndd(file):
    file.seek(5)
    sign = file.read(11)
    if (sign == b'\x1D\x46\xE5\xBD\x31\xEF\xE7\xFE\x74\xB7\x1D'):
        return (True, ".indd")
    else:
        return (False, "")


def isPng(file):
    file.seek(1)
    sign = file.read(3)
    if (sign == b'PNG'):
        return (True, ".png")
    else:
        return (False, "")


def isEps(file):
    file.seek(0)
    sign = file.read(4)
    if (sign == b'\xC5\xD0\xD3\xC6'):
        return (True, ".eps")
    else:
        return (False, "")


def InitCheckers(checkers):
    checkers.append(isDwgFile)
    checkers.append(isMp3)
    checkers.append(isFlac)
    checkers.append(isJpeg)
    checkers.append(isTiff)
    checkers.append(isMov)
    checkers.append(isEps)
    checkers.append(isPdf)
    checkers.append(isRtf)
    checkers.append(isAiff)
    checkers.append(isMkv)
    checkers.append(isAvi)
    checkers.append(isMSOffice2013)
    checkers.append(isRar)
    checkers.append(isZip) # zip, docx, xlsx, pptx
    checkers.append(isIndd)
    checkers.append(isPng)
    checkers.append(isEps)
    return checkers

def FileExtension(file_path):
    file_name = os.path.split(file_path)[-1]
    file_ext = os.path.splitext(file_name)[-1]
    return file_ext


def RenameFilesInDir(dir_path):

    #checkers = GetCheckers()

    dir_path = dir_path.strip()
    print("Processing:", dir_path)
    for file_name in os.listdir(dir_path):
        file_path = os.path.join(dir_path, file_name)
        if os.path.isdir(file_path):
            RenameFilesInDir(file_path)
        else:
            try:
                with open(file_path, "rb") as file:
                    # print(file.name)
                    for checker in checkers:
                        recognized, extension = checker(file)
                        if recognized is True:
                            file_ext = FileExtension(file_path)
                            if file_ext.lower() != extension.lower():
                                new_name = file_path + extension
                                file.close()
                                os.rename(file_path, new_name)
                                break
            except Exception as err:
                print("ERROR:", file_path)
                print(err)
                pass

def SSInternals(file_name):
    if olefile.isOleFile(file_name):
        ole = olefile.OleFileIO(file_name)
        for item in ole.listdir():
            print(item)


def ZipInternals(file_name):
    try:
        with ZipFile(file_name.strip(), "r") as zip_file:
            info_list = zip_file.infolist()
            for info in info_list:
                print(info)
            print("=========================")
            zip_file.printdir()
    except BadZipFile:
        pass


if __name__ == "__main__":
    # ZipInternals(r'')
    # type = ZipInternals(r'E:\42785\NoName\Files\zip\010983.zip')
    # SSInternals(r'E:\$LostFiles\NoName\000070 2')
    InitCheckers(checkers)
    RenameFilesInDir(PATH)
    print("Finished")
    pass