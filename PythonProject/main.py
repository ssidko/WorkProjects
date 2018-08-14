import os
import sys
import ZipTest
import subprocess
import time
#import nuuo

src_dir = r"F:\Recovered\DATA 2"
dst_dir = r"E:\41631\DATA-2"

processes_count = 3
childs = []

dirs = os.listdir(src_dir)
while len(dirs):

    while len(dirs) and len(childs) < processes_count:
        dir_dame = dirs.pop()
        dir_path = os.path.join(src_dir, dir_dame)

        if os.path.isdir(dir_path):
            cmd_line = [sys.executable, "nuuo.py", dst_dir, dir_path]
            child = subprocess.Popen(cmd_line)
            print("Run new process:", child)
            childs.append(child)

    to_remove = []
    for child in childs:
        if child.poll() is not None:
            to_remove.append(child)
            print("Process:", child, 'is terminated')

    for child in to_remove:
        print("Porcess:", child, "removed")
        childs.remove(child)

    time.sleep(1)

while childs:
    child = childs.pop()
    child.wait()

#nuuo.run(r"F:\Root\DATA 3", r"E:\41631\DATA-3")

