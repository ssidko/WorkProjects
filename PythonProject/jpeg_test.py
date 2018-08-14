import os
import PIL
from PIL import Image
from PIL.ExifTags import TAGS
from PIL.JpegImagePlugin import JpegImageFile
import time


PATH = r'D:\Work\38661\NoName\jpg'
BAD_FILE_EXT = '.bad_file'


class BadFile(Exception):
    pass


def MarkAsBad(file_path):
    try:
        bad_file_path = file_path + BAD_FILE_EXT
        os.rename(file_path, bad_file_path)
        print("Marked as BAD_FILE.")
    except Exception as err:
        print(err)


def FileExtension(file_path_or_name):
    path, file_name = os.path.split(file_path_or_name)
    name, ext = os.path.splitext(file_name)
    return ext


def TestJpegFile(file_path):
    with open(file_path, "rb") as file:
        try:
            picture = PIL.Image.open(file)

            '''
            info = picture._getexif()
            for tag, value in info.items():
                decoded = TAGS.get(tag, tag)
                print(decoded, ":", value)
            '''
            data = picture.getdata()
            # picture.copy()
        except Exception as exc:
            print(exc)
            raise BadFile()


def TestFile(file_path):
    file_ext = FileExtension(file_path)
    try:
        if file_ext.lower() == ".jpeg" or file_ext.lower() == ".jpg":
            TestJpegFile(file_path)
    except BadFile as exc:
        print("Bad File:", file_path)
        MarkAsBad(file_path)
    except Exception as exc:
        print(exc)


def ForEachFileInDir(dir_path, action, subdirs=False):
    print("processing directory:", dir_path)
    for file_name in os.listdir(dir_path):
        file_path = os.path.join(dir_path, file_name)
        if os.path.isdir(file_path) and subdirs:
            ForEachFileInDir(file_path, action, subdirs)
        else:
            action(file_path)


if __name__ == "__main__":
    print("--== Jpeg tester v.0.0.1 ==--")
    print("")
    print("*** Start ****")

    try:
        start = time.time()

        ForEachFileInDir(os.path.normpath(PATH), TestFile, True)

        duration = time.time() - start
        print("Elapsed time:", round(duration, 2))
    except Exception as exc:
        print(exc)

    input("*** Finished ***")