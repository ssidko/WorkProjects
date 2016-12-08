#include <windows.h>

#pragma pack(push)
#pragma pack(1)

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
	DWORD large_sectors;				/* zero */
} BIOS_PARAMETER_BLOCK;					/* sizeof() = 25 bytes. */

typedef struct {
	BYTE jump[3];						/* Irrelevant (jump to boot up code).*/
	ULONGLONG oem_id;					/* Magic "NTFS    ". */
	BIOS_PARAMETER_BLOCK bpb;			/* See BIOS_PARAMETER_BLOCK. */
	BYTE physical_drive;				/* 0x00 floppy, 0x80 hard disk */
	BYTE current_head;					/* zero */
	BYTE extended_boot_signature; 		/* 0x80 */
	BYTE reserved2;						/* zero */
	LONGLONG number_of_sectors;			/* Number of sectors in volume. Gives
											maximum volume size of 2^63 sectors.
											Assuming standard sector size of 512
											bytes, the maximum byte size is
											approx. 4.7x10^21 bytes. (-; */
	LONGLONG mft_lcn;					/* Cluster location of mft data. */
	LONGLONG mftmirr_lcn;				/* Cluster location of copy of mft. */
	char clusters_per_mft_record;		/* Mft record size in clusters. */
	BYTE reserved0[3];					/* zero */
	char clusters_per_index_record;		/* Index block size in clusters. */
	BYTE reserved1[3];					/* zero */
	ULONGLONG volume_serial_number;		/* Irrelevant (serial number). */
	DWORD checksum;						/* Boot sector checksum. */
	BYTE bootstrap[426];				/* Irrelevant (boot up code). */
	WORD end_of_sector_marker;			/* End of bootsector magic. Always is 0xaa55 in little endian. */
} NTFS_BOOT_SECTOR;						/* sizeof() = 512 bytes */

typedef enum {
	magic_FILE = (0x454c4946),			/* Mft entry. */
	magic_INDX = (0x58444e49),			/* Index buffer. */
	magic_HOLE = (0x454c4f48),			/* ? (NTFS 3.0+?) */
										/* Found in $LogFile/$DATA. */
	magic_RSTR = (0x52545352),			/* Restart page. */
	magic_RCRD = (0x44524352),			/* Log record page. */
										/* Found in $LogFile/$DATA.  (May be found in $MFT/$DATA, also?) */
	magic_CHKD = (0x444b4843),			/* Modified by chkdsk. */
										/* Found in all ntfs record containing records. */
	magic_BAAD = (0x44414142),			/* Failed multi sector transfer was detected. */

										/*
										 * Found in $LogFile/$DATA when a page is full or 0xff bytes and is
										 * thus not initialized.  User has to initialize the page before using
										 * it.
										 */
	magic_empty = (0xffffffff),			/* Record is empty and has to be initialized before it can be used. */
} NTFS_RECORD_TYPES;

/**
* enum MFT_RECORD_FLAGS -
*
* These are the so far known MFT_RECORD_* flags (16-bit) which contain
* information about the mft record in which they are present.
*
* MFT_RECORD_IS_4 exists on all $Extend sub-files.
* It seems that it marks it is a metadata file with MFT record >24, however,
* it is unknown if it is limited to metadata files only.
*
* MFT_RECORD_IS_VIEW_INDEX exists on every metafile with a non directory
* index, that means an INDEX_ROOT and an INDEX_ALLOCATION with a name other
* than "$I30". It is unknown if it is limited to metadata files only.
*/
typedef enum {
	MFT_RECORD_IN_USE			= (0x0001),
	MFT_RECORD_IS_DIRECTORY		= (0x0002),
	MFT_RECORD_IS_4				= (0x0004),
	MFT_RECORD_IS_VIEW_INDEX	= (0x0008),
	MFT_REC_SPACE_FILLER		= 0xffff,	 /* Just to make flags 16-bit. */
} MFT_RECORD_FLAGS;


typedef struct {
	DWORD magic;				/* Usually the magic is "FILE". */
	WORD usa_ofs;				/* See NTFS_RECORD definition above. */
	WORD usa_count;				/* See NTFS_RECORD definition above. */

	ULONGLONG lsn;				/* $LogFile sequence number for this record.
									Changed every time the record is modified. */
	WORD sequence_number;		/* Number of times this mft record has been
									reused. (See description for MFT_REF
									above.) NOTE: The increment (skipping zero)
									is done when the file is deleted. NOTE: If
									this is zero it is left zero. */
	WORD link_count;			/* Number of hard links, i.e. the number of
									directory entries referencing this record.
									NOTE: Only used in mft base records.
									NOTE: When deleting a directory entry we
									check the link_count and if it is 1 we
									delete the file. Otherwise we delete the
									FILE_NAME_ATTR being referenced by the
									directory entry from the mft record and
									decrement the link_count.
									FIXME: Careful with Win32 + DOS names! */
	WORD attrs_offset;			/* Byte offset to the first attribute in this
									mft record from the start of the mft record.
									NOTE: Must be aligned to 8-byte boundary. */
	WORD flags;					/* Bit array of MFT_RECORD_FLAGS. When a file
									is deleted, the MFT_RECORD_IN_USE flag is
									set to zero. */
	DWORD bytes_in_use;			/* Number of bytes used in this mft record.
									NOTE: Must be aligned to 8-byte boundary. */
	DWORD bytes_allocated;		/* Number of bytes allocated for this mft
									record. This should be equal to the mft
									record size. */
	ULONGLONG base_mft_record;	/* This is zero for base mft records.
									When it is not zero it is a mft reference
									pointing to the base mft record to which
									this record belongs (this is then used to
									locate the attribute list attribute present
									in the base record which describes this
									extension record and hence might need
									modification when the extension record
									itself is modified, also locating the
									attribute list also means finding the other
									potential extents, belonging to the non-base
									mft record). */
	WORD next_attr_instance;	/* The instance number that will be
									assigned to the next attribute added to this
									mft record. NOTE: Incremented each time
									after it is used. NOTE: Every time the mft
									record is reused this number is set to zero.
									NOTE: The first instance number is always 0. */
	
	/* The below fields are specific to NTFS 3.1+ (Windows XP and above): */
	
	WORD reserved;				/* Reserved/alignment. */
	DWORD mft_record_number;	/* Number of this mft record. */
									
								/*
								* When (re)using the mft record, we place the update sequence array at this
								* offset, i.e. before we start with the attributes. This also makes sense,
								* otherwise we could run into problems with the update sequence array
								* containing in itself the last two bytes of a sector which would mean that
								* multi sector transfer protection wouldn't work. As you can't protect data
								* by overwriting it since you then can't get it back...
								* When reading we obviously use the data from the ntfs record header.
								*/
} MFT_RECORD;					/* sizeof() = 48 bytes */

typedef enum {
	ATTR_UNUSED							= 0x00,
	ATTR_STANDARD_INFORMATION			= 0x10,
	ATTR_ATTRIBUTE_LIST					= 0x20,
	ATTR_FILE_NAME						= 0x30,
	ATTR_OBJECT_ID						= 0x40,
	ATTR_SECURITY_DESCRIPTOR			= 0x50,
	ATTR_VOLUME_NAME					= 0x60,
	ATTR_VOLUME_INFORMATION				= 0x70,
	ATTR_DATA							= 0x80,
	ATTR_INDEX_ROOT						= 0x90,
	ATTR_INDEX_ALLOCATION				= 0xa0,
	ATTR_BITMAP							= 0xb0,
	ATTR_REPARSE_POINT					= 0xc0,
	ATTR_EA_INFORMATION					= 0xd0,
	ATTR_EA								= 0xe0,
	ATTR_PROPERTY_SET					= 0xf0,
	ATTR_LOGGED_UTILITY_STREAM			= 0x100,
	ATTR_FIRST_USER_DEFINED_ATTRIBUTE	= 0x1000,
	ATTR_END							= 0xFFFFFFFF,
} ATTR_TYPES;

typedef struct {
	DWORD attr_type;				/* ATTR_TYPES. The (32-bit) type of the attribute. */
	DWORD length;					/* Byte size of the resident part of the
										attribute (aligned to 8-byte boundary).
										Used to get to the next attribute. */
	BYTE non_resident;				/* If 0, attribute is resident. If 1, attribute is non-resident. */
	BYTE name_length;				/* Unicode character size of name of attribute. 0 if unnamed. */
	WORD name_offset;				/* If name_length != 0, the byte offset to the
										beginning of the name from the attribute
										record. Note that the name is stored as a
										Unicode string. When creating, place offset
										just at the end of the record header. Then,
										follow with attribute value or mapping pairs
										array, resident and non-resident attributes
										respectively, aligning to an 8-byte
										boundary. */
	WORD attr_flag;					/* Flags describing the attribute. */
	WORD instance;					/* The instance of this attribute record. This
										number is unique within this mft record (see
										MFT_RECORD/next_attribute_instance notes
										above for more details). */
	union {

		struct {
			DWORD value_length;		/* Byte size of attribute value. */
			WORD value_offset;		/* Byte offset of the attribute value from the start of the
										attribute record. When creating, align to 8-byte boundary if we
										have a name present as this might not have a length of a multiple of 8-bytes. */
			BYTE resident_attr_flags; /* See above. */
			char reserved;			/* Reserved/alignment to 8-byte boundary. */
		} resident_attr;

		struct {
			ULONGLONG lowest_ULONGLONG;	/* Lowest valid virtual cluster number
											for this portion of the attribute value or
											0 if this is the only extent (usually the
											case). - Only when an attribute list is used
											does lowest_ULONGLONG != 0 ever occur. */
			ULONGLONG highest_ULONGLONG; /* Highest valid ULONGLONG of this extent of
											 the attribute value. - Usually there is only one
											 portion, so this usually equals the attribute
											 value size in clusters minus 1. Can be -1 for
											 zero length files. Can be 0 for "single extent"
											 attributes. */
			WORD mapping_pairs_offset;				/* Byte offset from the
													 beginning of the structure to the mapping pairs
													 array which contains the mappings between the
													 ULONGLONGs and the logical cluster numbers (lcns).
													 When creating, place this at the end of this
													 record header aligned to 8-byte boundary. */
			BYTE compression_unit;					/* The compression unit expressed
													 as the log to the base 2 of the number of
													 clusters in a compression unit. 0 means not
													 compressed. (This effectively limits the
													 compression unit size to be a power of two
													 clusters.) WinNT4 only uses a value of 4. */
			BYTE reserved1[5];					/* Align to 8-byte boundary. */
													/* The sizes below are only used when lowest_ULONGLONG is zero, as otherwise it would
													be difficult to keep them up-to-date.*/
			LONGLONG allocated_size;			/* Byte size of disk space
													allocated to hold the attribute value. Always
													is a multiple of the cluster size. When a file
													is compressed, this field is a multiple of the
													compression block size (2^compression_unit) and
													it represents the logically allocated space
													rather than the actual on disk usage. For this
													use the compressed_size (see below). */
			LONGLONG data_size;						/* Byte size of the attribute
													value. Can be larger than allocated_size if
													attribute value is compressed or sparse. */
			LONGLONG initialized_size;			/* Byte size of initialized
													portion of the attribute value. Usually equals
													data_size. */
			LONGLONG compressed_size;			/* Byte size of the attribute
													value after compression. Only present when
													compressed. Always is a multiple of the
													cluster size. Represents the actual amount of
													disk space being used on the disk. */
			//void *compressed_end[0];
		} non_resident_attr;
	} ;
} ATTR_RECORD;


#pragma pack(pop)