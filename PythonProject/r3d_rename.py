import os

PATH = "F:\\raw"
R3D_EXT = ".R3D"
OFFSET = 744
#OFFSET = 688
#OFFSET = 704

if __name__ == "__main__":
    for file_name in os.listdir(PATH):
        file_path = os.path.join(PATH, file_name)
        print(file_path)
        if os.path.isdir(file_path):
           break
        if not file_name.endswith(R3D_EXT):
            break
        new_file_name = ""
        with open(file_path, "rb") as file:
            file.seek(OFFSET)
            new_file_name = file.read(24).decode("windows-1252")

        counter = 0
        if new_file_name.endswith(R3D_EXT):
            new_file_name_path = os.path.join(PATH, new_file_name)
            if os.path.exists(new_file_name_path):
                new_file_name_path = os.path.join(PATH, str(counter) + "-" +  new_file_name)
                counter += 1

            print("Rename:", file_path, "to", new_file_name_path)
            os.rename(file_path, new_file_name_path)
