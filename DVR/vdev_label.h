#ifndef _VDEV_LABEL_H
#define _VDEV_LABEL_H

#include <cstdint>
#include "spa.h"

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

#define UBERBLOCK_SHIFT		10

#define	VDEV_PAD_SIZE		(8 << 10)				/*  8K */
#define	VDEV_SKIP_SIZE		VDEV_PAD_SIZE * 2		/* 2 padding areas (vl_pad1 and vl_pad2) to skip */
#define	VDEV_PHYS_SIZE		(112 << 10)
#define	VDEV_UBERBLOCK_RING	(128 << 10)

typedef struct vdev_phys {
	char		vp_nvlist[VDEV_PHYS_SIZE - sizeof(zio_eck_t)];
	zio_eck_t	vp_zbt;
} vdev_phys_t;

typedef struct vdev_label {
	char		vl_pad1[VDEV_PAD_SIZE];				/*  8K */
	char		vl_pad2[VDEV_PAD_SIZE];				/*  8K */
	vdev_phys_t	vl_vdev_phys;						/* 112K	*/
	char		vl_uberblock[VDEV_UBERBLOCK_RING];	/* 128K	*/
} vdev_label_t;										/* 256K total */

/* Offset of embedded boot loader region on each label */
#define	VDEV_BOOT_OFFSET	(2 * sizeof (vdev_label_t))

/*
* Size of embedded boot loader region on each label.
* The total size of the first two labels plus the boot area is 4MB.
*/
#define	VDEV_BOOT_SIZE		(7ULL << 19)			/* 3.5M */

/*
* Size of label regions at the start and end of each leaf device.
*/
#define	VDEV_LABEL_START_SIZE	(2 * sizeof (vdev_label_t) + VDEV_BOOT_SIZE)
#define	VDEV_LABEL_END_SIZE		(2 * sizeof (vdev_label_t))
#define	VDEV_LABELS				4
#define	VDEV_BEST_LABEL			VDEV_LABELS

#endif // _VDEV_LABEL_H