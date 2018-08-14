import os
import zlib

def decompress(compressed_file_name, out_file_name):
    with open(compressed_file_name, "rb") as compressed_file:
        with open(out_file_name,"wb+") as out_file:
            data = compressed_file.read()
            if data[0:1] == b'\x78':
                decompressed = zlib.decompress(data)
                out_file.write(decompressed)
                pass
            #return decompressed
            pass
    pass

if __name__ == "__main__":

    to_compress_file_name = "D:\\Work\\42301\\p3.scn"
    compresssed_file_name = "D:\\Work\\42301\\out.zlib"

    decompressed = decompress(r'E:\noname1', r'E:\noname.bin')

    """
    with open (to_compress_file_name, "rb") as src_file:
        data = src_file.read()
        print("Original size:", len(data))

        compressed_data = zlib.compress(data)
        print("Compressed size:", len(compressed_data))
        with open(compresssed_file_name, "wb") as out_file:
            out_file.write(compressed_data)
    """

    print("Finished")

    pass