import os
import zipfile


def LastModifiedYear(file_path):
    with zipfile.ZipFile(file_path) as arch:
        for info in arch.infolist():
            if (info.filename.lower().endswith('.1cd') or info.filename.lower().endswith(".dt") or info.filename.lower().endswith(".zbk")):
                return info.date_time[0]
    return 0

def ZipTest(path):
    for file_name in os.listdir(path):
        if (file_name.endswith('.zip')):
            arch_path = path + file_name
            print('-----------------------------------')
            print(file_name)
            print('-----------------------------------')

            try:
                mod_year = LastModifiedYear(arch_path)
                if (mod_year > 0):
                    os.rename(arch_path, path + str(mod_year) + "-" + file_name)
            except zipfile.BadZipFile:
                os.rename(arch_path, path + "bad-" + file_name)
