import os
import sys
import zipfile
import olefile
import xml.etree.ElementTree as ET

PATH = r'E:\44749\NoName'

PROPS_AUTHOR = "author"
PROPS_TITLE = "title"
PROPS_CREATED = "created"
PROPS_MODIFIED = "modified"

BAD_FILE_EXT = ".bad_file"

file_counter = 0


class BadFile(Exception):
    pass


def OleProps(file_path):
    props = dict()
    if olefile.isOleFile(file_path):
        ole = olefile.OleFileIO(file_path)
        meta = ole.get_metadata()
        if meta:
            props[PROPS_AUTHOR] = meta.author
            props[PROPS_TITLE] = meta.title
            props[PROPS_CREATED] = str(meta.create_time)
            props[PROPS_MODIFIED] = str(meta.last_saved_time)
        ole.close()
    return props


def OpenXMLProps(file_path):
    props = dict()
    with zipfile.ZipFile(file_path, "r") as zip_file:

        #print(zip_file.namelist())

        props_xml = zip_file.open("docProps/core.xml", "r")
        xml_data = props_xml.read()
        xml_str = str(xml_data, encoding="utf-8")
        root = ET.fromstring(xml_str)

        #print("tag:", root.tag, "attribute:", root.attrib)

        for child in root:
            #print ("tag:", child.tag, "attribute:", child.attrib)
            if child.tag.find('creator') >= 0:
                #print("Creator: ", child.text)
                props[PROPS_AUTHOR] = child.text
                continue
            if child.tag.find('title') >= 0:
                #print("Title: ", child.text)
                props[PROPS_TITLE] = child.text
                continue
            if child.tag.find('created') >= 0:
                #print("created: ", child.text)
                props[PROPS_CREATED] = child.text
                continue
            if child.tag.find('modified') >= 0:
                #print("modified: ", child.text)
                props[PROPS_MODIFIED] = child.text
                continue
        return props


def FileExtension(file_path_or_name):
    path, file_name = os.path.split(file_path_or_name)
    name, ext = os.path.splitext(file_name)
    return ext


def SplitFilePath(file_path_or_name):
    path, file_name = os.path.split(file_path_or_name)
    name, ext = os.path.splitext(file_name)
    return (path, name, ext)


def MarkAsBad(file_path):
    try:
        bad_file_path = file_path + BAD_FILE_EXT
        os.rename(file_path, bad_file_path)
        print("Marked as BAD_FILE.")
    except Exception as err:
        print(err)


def TestMsOffice2003File(file_path):
    with open(file_path, "rb") as file:
        data = file.read()
        try:
            ole = olefile.OleFileIO(data, "r")
            stream_name = str()
            if ole.exists('workbook'):
                stream_name = "workbook"
            elif ole.exists("WordDocument"):
                stream_name = "WordDocument"
            elif ole.exists("PowerPoint Document"):
                stream_name = "PowerPoint Document"
            else:
                raise BadFile()

            size = ole.get_size(stream_name)
            stream = ole.openstream(stream_name)
            data = stream.read()

            # print(stream_name)
            # print("size:", size)
        except:
            raise BadFile()

def TestMsOfficeOpenXML(file_path):
    try:
        with zipfile.ZipFile(file_path, "r") as zip_file:
            zip_file.testzip()
    except:
        raise BadFile()


def PrepareDateTime(date_time):
    date_time = date_time.replace(":", "-")
    date_time = date_time.upper()
    date_time = date_time.replace("T", " ")
    date_time = date_time.replace("Z", "")
    return date_time


def RenameInDir(dir_path):

    assert os.path.isdir(dir_path)

    file_counter = 0

    for file_name in os.listdir(dir_path):

        file_counter += 1

        file_path = os.path.join(dir_path, file_name)
        if os.path.isdir(file_path):
            RenameInDir(file_path)
            continue

        file_ext = FileExtension(file_name).lower()

        try:
            props = dict()
            if file_name.lower().endswith(".doc", ".xls", ".ppt"):
                props = OleProps(file_path)
            elif file_name.lower().endswith(".docx", ".xlsx", ".pptx"):
                props = OpenXMLProps(file_path)
            else:
                continue

            name, ext = os.path.splitext(file_name)
            new_file_name = "[%s]-%06d%s" % (PrepareDateTime([PROPS_MODIFIED]), file_counter, ext)
            new_file_path = os.path.join(dir_path, new_file_name)

            os.rename(file_path, new_file_path)

        except BadFile:
            if not os.path.isdir(file_path):
                print("BAD File:", file_path)
                MarkAsBad(file_path)
        except Exception as exc:
            print("Error:", file_path)
            print("Reason:", exc)


def IntegrityCheckAndRename(file_path):
    if FileExtension(file_path).lower().endswith((".doc", ".docx", ".xls", ".xlsx", ".ppt", ".pptx")):
        try:

            if olefile.isOleFile(file_path):
                TestMsOffice2003File(file_path)
                props = OleProps(file_path)
            elif zipfile.is_zipfile(file_path):
                TestMsOfficeOpenXML(file_path)
                props = OpenXMLProps(file_path)
            else:
                raise BadFile()

            if props:
                path, name, ext = SplitFilePath(file_path)
                new_file_name = "[%s]-%s%s" % (PrepareDateTime(props[PROPS_MODIFIED]), name, ext)
                new_file_path = os.path.join(path, new_file_name)
                os.rename(file_path, new_file_path)

        except BadFile as exc:
            print("BAD File:", file_path)
            MarkAsBad(file_path)
        except Exception as exc:
            print("Error:", file_path)
            print("Reason:", exc)


def ForEachFileInDir(dir_path, action, subdirs = False):
    for file_name in os.listdir(dir_path):
        file_path = os.path.join(dir_path, file_name)
        if os.path.isdir(file_path) and subdirs:
            ForEachFileInDir(file_path, action, subdirs)
        else:
            action(file_path)


if __name__ == "__main__":

    try:
        ForEachFileInDir(PATH.strip(), IntegrityCheckAndRename, subdirs=True)
        # IntegrityCheckAndRename(r"\\servergiga\Заказы\42980\MS Office\000878.pptx")
        # file_name = r'\\servergiga\Заказы\43057\NoName\xls(x)\034879.xlsx'
        # file_name = r' F:\43057\noname.zip'.strip()
        # TestMsOfficeOpenXML(file_name)
        # TestMsOffice2003File(file_name)
        # props = OleProps(file_name)
        # RenameInDir(PATH)

    except Exception as err:
        print(err)

    print("Finished!")
