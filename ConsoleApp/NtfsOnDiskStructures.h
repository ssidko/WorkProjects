
#include <windows.h>

#pragma pack(push)
#pragma pack(1)

/*
*	Location of bootsector on partition:
*	The standard NTFS_BOOT_SECTOR is on sector 0 of the partition.
*	On NT4 and above there is one backup copy of the boot sector to
*	be found on the last sector of the partition (not normally accessible
*	from within Windows as the bootsector contained number of sectors
*	value is one less than the actual value!).
*	On versions of NT 3.51 and earlier, the backup copy was located at
*	number of sectors/2 (integer divide), i.e. in the middle of the volume.
*/

typedef struct {
	WORD bytes_per_sector;				/* Size of a sector in bytes. */
	BYTE sectors_per_cluster;			/* Size of a cluster in sectors. */
	WORD reserved_sectors;				/* zero */
	BYTE fats;							/* zero */
	WORD root_entries;					/* zero */
	WORD sectors;						/* zero */
	BYTE media_type;					/* 0xf8 = hard disk */
	WORD sectors_per_fat;				/* zero */
	WORD sectors_per_track;				/* Required to boot Windows. */
	WORD heads;							/* Required to boot Windows. */
	DWORD hidden_sectors;				/* Offset to the start of the partition
										   relative to the disk in sectors.
										   Required to boot Windows. */
	DWORD large_sectors;				/* zero 
										   sizeof() = 25 (0x19) bytes */
} BIOS_PARAMETER_BLOCK;	

typedef struct {
	BYTE  jump[3];						/* Irrelevant (jump to boot up code).*/
	ULONGLONG oem_id;					/* Magic "NTFS    ". */
	BIOS_PARAMETER_BLOCK bpb;			/* See BIOS_PARAMETER_BLOCK. */
	BYTE physical_drive;				/* 0x00 floppy, 0x80 hard disk */
	BYTE current_head;					/* zero */
	BYTE extended_boot_signature;		/* 0x80 */
	BYTE reserved2;						/* zero */
	LONGLONG number_of_sectors;			/* Number of sectors in volume. Gives
										   maximum volume size of 2^63 sectors.
										   Assuming standard sector size of 512
										   bytes, the maximum byte size is
										   approx. 4.7x10^21 bytes. (-; */
	LONGLONG mft_lcn;					/* Cluster location of mft data. */
	LONGLONG mftmirr_lcn;				/* Cluster location of copy of mft. */
	BYTE clusters_per_mft_record;		/* Mft record size in clusters. */
	BYTE reserved0[3];					/* zero */
	BYTE clusters_per_index_record;		/* Index block size in clusters. */
	BYTE reserved1[3];					/* zero */
	ULONGLONG volume_serial_number;		/* Irrelevant (serial number). */
	DWORD checksum;						/* Boot sector checksum. */
	BYTE bootstrap[426];				/* Irrelevant (boot up code). */
	WORD end_of_sector_marker;			/* End of bootsector magic. Always is
										   0xaa55 in little endian.
										   sizeof() = 512 (0x200) bytes */
} NTFS_BOOT_SECTOR;

#pragma pack(pop)

typedef enum {
	/* Found in $MFT/$DATA. */
	magic_FILE = 0x454c4946,			/* Mft entry. */
	magic_INDX = 0x58444e49,			/* Index buffer. */
	magic_HOLE = 0x454c4f48,			/* ? (NTFS 3.0+?) */

	/* Found in $LogFile/$DATA. */
	magic_RSTR = 0x52545352,			/* Restart page. */
	magic_RCRD = 0x44524352,			/* Log record page. */

	/* Found in $LogFile/$DATA.  (May be found in $MFT/$DATA, also?) */
	magic_CHKD = 0x444b4843,			/* Modified by chkdsk. */

	/* Found in all ntfs record containing records. */
	magic_BAAD = 0x44414142,			/* Failed multi sector transfer was detected. */

	/*
	* Found in $LogFile/$DATA when a page is full or 0xff bytes and is
	* thus not initialized.  User has to initialize the page before using
	* it.
	*/
	magic_empty = 0xffffffff,			/* Record is empty and has to be initialized before it can be used. */
} NTFS_RECORD_TYPES;