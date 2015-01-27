#pragma once

namespace fat {

#define LAST_LONG_ENTRY			(0x40)

	typedef enum {
		kReadOnly		= 1,
		kHidden			= 1 << 1,
		kSystem			= 1 << 2,
		kVolumeID		= 1 << 3,
		kDirectory		= 1 << 4,
		kArchive		= 1 << 5,
		kLongName		= kReadOnly|kHidden|kSystem|kVolumeID,
		kAttributesMask	= kLongName|kDirectory|kArchive,
	} Attributes;

#pragma pack(push)
#pragma pack(1)

	typedef struct _DATE {
		unsigned short day : 5;		// Day of month, valid range [1-31].
		unsigned short month : 4;	// Month of year, valid range [1-12].
		unsigned short year : 7;	// Count of years from 1980, valid range [0-127] (1980-2107).
	} DATE;

	typedef struct _TIME {
		unsigned short seconds : 5;	// valid range [0-29].
		unsigned short minutes : 6;	// valid range [0-59].
		unsigned short hours : 5;	// valid range [0-23].
	} TIME;

	typedef struct _DIR_ENTRY {
		unsigned char name[11];
		unsigned char attributes;
		unsigned char nt_reserved;
		unsigned char creation_time_tenth;
		TIME creation_time;
		DATE creation_date;
		unsigned short access_date;
		unsigned short first_cluster_hi;
		TIME write_time;
		DATE write_date;
		unsigned short first_cluster_lo;
		unsigned int file_size;
	} DIR_ENTRY;

	typedef struct _DIR_ENTRY_LONG {
		unsigned char order;
		unsigned char name_1_5[10];
		unsigned char attributes;
		unsigned char type;
		unsigned char checksum;
		unsigned char name_6_11[12];
		unsigned short first_cluster_lo;		// must be 0x0000
		unsigned char name_12_13[4];
	} DIR_ENTRY_LONG;

#pragma pack(pop)

	bool IsValidDirEntry(char *buff);

	class Fat
	{
	public:
		Fat(void);
		~Fat(void);
	};

}
