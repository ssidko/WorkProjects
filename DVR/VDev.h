#ifndef _ZFS_H
#define _ZFS_H

#include <string>

/*
|<------------------- VDEV Label (L0) ------------------------>|
|                                        |<- Uberblock Array ->|
+===========+===========+================+=+=+=+=========+=+=+=+=========+==========+===================+========+========+
|           |           |                | | | |         | | | |         |   		|				    |        |        |
|Blank Space|Boot Header|Name/Value Pairs| | | |  .....  | | | |   L1    |Boot Block|    DATA BLOCKS    |   L2   |   L3   |
|           |           |                | | | |         | | | |         |   		|				    |        |        |
+===========+===========+================+=+=+=+=========+=+=+=+=========+==========+===================+========+========+
0          8K	       16K              128K                  256K      512K       4M	     	     N-512K   N-256K      N
*/

#define UBERBLOCK_SIZE						(0x400)

#define VDEV_LABEL_BLANK_SPACE_SIZE			(0x2000)
#define VDEV_LABEL_BOOT_HEADER_SIZE			(0x2000)
#define VDEV_LABEL_NV_PAIRS_SIZE			(0x1C000)
#define VDEV_LABEL_UBERBLOCK_ARRAY_SIZE		(128*1024)
#define VDEV_LABEL_SIZE						(VDEV_LABEL_BLANK_SPACE_SIZE+VDEV_LABEL_BOOT_HEADER_SIZE+VDEV_LABEL_NV_PAIRS_SIZE+VDEV_LABEL_UBERBLOCK_ARRAY_SIZE)

#define VDEV_BOOT_BLOCK_SIZE				(0x380000)

#define VDEV_LABEL_NV_PAIRS_OFFSET			(VDEV_LABEL_BLANK_SPACE_SIZE+VDEV_LABEL_BOOT_HEADER_SIZE)
#define VDEV_LABEL_UBERBLOCKS_OFFSET		(VDEV_LABEL_NV_PAIRS_OFFSET+VDEV_LABEL_NV_PAIRS_SIZE)	
#define VDEV_DATA_OFFSET					(2*VDEV_LABEL_SIZE+VDEV_BOOT_BLOCK_SIZE)


class VDev
{
private:
	unsigned int block_size;
	std::string file_name;
public:
	VDev();
	~VDev();
};

#endif // _ZFS_H