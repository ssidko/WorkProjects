#pragma once
#include "stdafx.h"

#define LOCAL_FILE_HEADER_SIGN				(DWORD)0x04034b50
#define DATA_DESCRIPTOR_SIGN				(DWORD)0x08074b50
#define CENTRAL_FILE_HEADER_SIGN			(DWORD)0x02014b50
#define DIGITAL_SIGNATURE_SIGN				(DWORD)0x05054b50 
#define END_CDIRECTORY_RECORD_SIGN			(DWORD)0x06054b50

#define LOCAL_FILE_HEADER_SIZE				(uint32_t)30 

#pragma pack(1)

typedef struct _LOCAL_FILE_HEADER_32 {
	DWORD signature;						// local file header signature LOCAL_FILE_HEADER_SIGN (0x04034b50)
	WORD ver_needed;						// version needed to extract 
	WORD flag;								// general purpose bit flag 
	WORD compr_method;						// compression method 
    WORD time;								// last mod file time 
    WORD date;								// last mod file date
	DWORD crc32;							// crc-32
	DWORD compr_size;						// compressed size 
	DWORD uncompr_size;						// uncompressed size
	WORD name_len;							// file name length
	WORD extra_field_len;					// extra field length
	BYTE file_name[1];						// file name (variable size)

	bool isHasDataDescriptor() { return (flag & (1 << 3)) != 0; }
} LOCAL_FILE_HEADER_32;

typedef struct _DATA_DESCRIPTOR_32 {
	DWORD signature;						// DATA_DESCRYPTOR_SIGN (0x08074b50)
	DWORD crc32;							// crc-32
	DWORD compr_size;						// compressed size 
	DWORD uncompr_size;						// uncompressed size
} DATA_DESCRIPTOR_32;

typedef struct _DATA_DESCRIPTOR_64 {
	DWORD signature;						// DATA_DESCRYPTOR_SIGN (0x08074b50)
	DWORD crc32;							// crc-32
	LONGLONG compr_size;					// compressed size 
	LONGLONG uncompr_size;					// uncompressed size
} DATA_DESCRIPTOR_64;

typedef struct _CENTRAL_FILE_HEADER_32 {
	DWORD signature;						// central file header signature CENTRAL_FILE_HEADER_SIGN (0x02014b50)
	WORD ver_made;							// version made by
	WORD ver_needed;						// version needed to extract
	WORD flag;								// general purpose bit flag
	WORD compr_method;						// compression method
	WORD time;								// last mod file time
	WORD date;								// last mod file date
	DWORD crc32;							// crc-32
	DWORD compr_size;						// compressed size 
	DWORD uncompr_size;						// uncompressed size
	WORD name_len;							// file name length
	WORD extra_field_len;					// extra field length
	WORD comment_len;						// file comment length
	WORD disk_mun;							// disk number start
	WORD int_file_attr;						// internal file attributes
	DWORD ext_file_attr;					// external file attributes
	DWORD offset_local_header;				// relative offset of local header
	BYTE file_name[1];						// file name (variable size)
} CENTRAL_FILE_HEADER_32;

typedef struct _DIGITAL_SIGNATURE {
	DWORD signature;						// header signature DIGITAL_SIGNATURE_SIGN (0x05054b50)
	WORD data_size;							// size of data
	BYTE data[1];							// signature data (variable size)
} DIGITAL_SIGNATURE;

typedef struct _END_OF_CDIRECTORY_RECORD_32 {
	DWORD signature;						// end of central dir signature END_CDIRECTORY_RECORD_SIGN (0x06054b50)
	WORD disk_num;							// number of this disk
	WORD cdir_disk_num;						// number of the disk with the start of the central directory
	WORD cdir_entries_count;				// total number of entries in the central directory on this disk				
	WORD cdir_entries_total_count;			// total number of entries in the central directory
	DWORD cdir_size;						// size of the central directory
	DWORD cdir_offset;						// offset of start of central directory with respect to the starting disk number
	DWORD comment_len;						// .ZIP file comment length
	BYTE comment[1];						// .ZIP file comment 
} END_OF_CDIRECTORY_RECORD_32;
#pragma pack()

class LocalFile {
private:
	FileEx *archive;
	LONGLONG offset;
	
	LONGLONG compr_size;
	LONGLONG uncompr_size;


public:
	LocalFile(FileEx *arc_file, LONGLONG offs) : archive(arc_file), offset(offs), compr_size(0), uncompr_size(0) {}
	~LocalFile() {};

	BOOL Initialize();
	void Clean();
};

int ExtractArchive(FileEx *archive, const TCHAR *out_dir);
int ZipRec_Main(int argc, _TCHAR* argv[]);
int TestZipRec(void);
