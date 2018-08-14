import os
import sys
from PyPDF2 import PdfFileReader
from PyPDF2.utils import PdfReadError


PATH = r'E:\44749\NoName\pdf'
PDF_FILE_EXT = ".pdf"
BAD_FILE_EXT = ".bad_file"


def MarkAsBad(file_path):
    try:
        bad_file_path = file_path + BAD_FILE_EXT
        os.rename(file_path, bad_file_path)
        print("Marked as BAD_FILE.")
    except:
        print("Error renaming file.")


if __name__ == "__main__":

    try:
        for file_name in os.listdir(os.path.normpath(PATH)):
            file_path = os.path.join(PATH, file_name)
            if os.path.isdir(file_path):
                continue
            if not file_name.upper().endswith(PDF_FILE_EXT.upper()):
                continue

            print("File: %s" % (file_path))

            #file_path = r'D:\Work\42367\NoName\pdf_bad\000349.pdf'

            try:
                with open(file_path, "rb") as file:
                    reader = PdfFileReader(file, strict=False)
                    #contents = reader.getPage(0).extractText().split('\n')
                    print("GOOD")

            except PdfReadError:
                print("Bad PDF file.")
                #file.close()
                MarkAsBad(file_path)
            except ValueError:
                print("Bad PDF file.")
                #file.close()
                MarkAsBad(file_path)
            except TypeError:
                print("Bad PDF file.")
                #file.close()
                MarkAsBad(file_path)
            except :
                print("Bad PDF file.")
                #file.close()
                MarkAsBad(file_path)

        print("Finished.")

    except FileNotFoundError:
        print("Not found:", PATH)