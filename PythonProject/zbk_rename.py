import os
import struct

PATH = r'D:\Work\42301\medoc'
ZBK_EXT = ".ZBK"

if __name__ == "__main__":

    counter = 0

    try:
        for file_name in os.listdir(PATH):
            file_path = os.path.join(PATH, file_name)
            print(file_path)
            if os.path.isdir(file_path):
               break
            if not file_name.upper().endswith(ZBK_EXT.upper()):
                break

            data = None

            with open(file_path, "rb") as file:
                try:
                    file.seek(16)
                    raw_data = file.read(4)
                    data = struct.unpack("<BBH", raw_data)
                except:
                    print("Error read file: %s" % (file_path))

            new_file_name = "%d-%d-%d.ZBK" % (data[2], data[1], data[0])
            new_file_name_path = os.path.join(PATH, new_file_name)

            if os.path.exists(new_file_name_path):
                new_file_name_path = os.path.join(PATH, str(counter) + "-" + new_file_name)
                counter += 1

            print("Rename:", file_path, "to", new_file_name_path)

            try:
                os.rename(file_path, new_file_name_path)
            except:
                print("Error rename file.")

            pass

            pass
    except:
        print("Error\n")