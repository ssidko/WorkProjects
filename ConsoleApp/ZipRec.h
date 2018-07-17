#pragma once
#include "stdafx.h"
#include <map>
#include <optional>

#define ZIP_CONSTANT_MARKER					(uint16_t)0x4b50 // "PK"
#define LOCAL_FILE_HEADER_SIGNATURE			(uint32_t)0x04034b50
#define DATA_DESCRIPTOR_SIGN				(uint32_t)0x08074b50
#define CENTRAL_FILE_HEADER_SIGN			(uint32_t)0x02014b50
#define DIGITAL_SIGNATURE_SIGN				(uint32_t)0x05054b50 
#define END_CDIRECTORY_RECORD_SIGN			(uint32_t)0x06054b50
#define ZIP64_END_CDIRECTORY_RECORD_SIGN	(uint32_t)0x06064b50

#define LOCAL_FILE_HEADER_SIZE				(uint32_t)30 

#pragma pack(push,1)

typedef struct _LOCAL_FILE_HEADER {
	uint32_t signature;						// local file header signature LOCAL_FILE_HEADER_SIGNATURE (0x04034b50)
	uint16_t ver_needed;					// version needed to extract 
	uint16_t flag;							// general purpose bit flag 
	uint16_t compr_method;					// compression method 
    uint16_t time;							// last mod file time 
    uint16_t date;							// last mod file date
	uint32_t crc32;							// crc-32
	uint32_t compr_size;					// compressed size 
	uint32_t uncompr_size;					// uncompressed size
	uint16_t name_len;						// file name length
	uint16_t extra_field_len;				// extra field length
	uint8_t file_name[1];					// file name (variable size)

	//
	// file_name[name_len]
	// extra_field[extra_field_len]
	//

	size_t SizeOfFixedFields() { return LOCAL_FILE_HEADER_SIZE; }
	size_t SizeOfVariableData() { return name_len + extra_field_len; }
	size_t TotalHeaderSize() { return SizeOfFixedFields() + SizeOfVariableData(); }

	bool DataDescriptorPresent() { return (flag & (1 << 3)) != 0; }
	bool EncriptionEnabled() { return (flag & (1 << 0)) != 0; }
	bool UTF8Encoding() { return (flag & (1 << 11)) != 0; }
} LOCAL_FILE_HEADER;

typedef struct _DATA_DESCRIPTOR {
	uint32_t signature;						// DATA_DESCRYPTOR_SIGN (0x08074b50)
	uint32_t crc32;							// crc-32
	uint32_t compr_size;					// compressed size 
	uint32_t uncompr_size;					// uncompressed size
} DATA_DESCRIPTOR;

typedef struct _ZIP64_DATA_DESCRIPTOR {
	uint32_t signature;						// DATA_DESCRYPTOR_SIGN (0x08074b50)
	uint32_t crc32;							// crc-32
	uint64_t compr_size;					// compressed size 
	uint64_t uncompr_size;					// uncompressed size
} ZIP64_DATA_DESCRIPTOR;

typedef struct _CDIR_HEADER {
	uint32_t signature;						// central file header signature CENTRAL_FILE_HEADER_SIGN (0x02014b50)
	uint16_t ver_made;						// version made by
	uint16_t ver_needed;					// version needed to extract
	uint16_t flag;							// general purpose bit flag
	uint16_t compr_method;					// compression method
	uint16_t time;							// last mod file time
	uint16_t date;							// last mod file date
	uint32_t crc32;							// crc-32
	uint32_t compr_size;					// compressed size 
	uint32_t uncompr_size;					// uncompressed size
	uint16_t name_len;						// file name length
	uint16_t extra_field_len;				// extra field length
	uint16_t comment_len;					// file comment length
	uint16_t disk_mun;						// disk number start
	uint16_t int_file_attr;					// internal file attributes
	uint32_t ext_file_attr;					// external file attributes
	uint32_t offset_local_header;			// relative offset of local header
	uint8_t file_name[1];					// file name (variable size)
} CDIR_HEADER;

typedef struct _DIGITAL_SIGNATURE {
	uint32_t signature;						// header signature DIGITAL_SIGNATURE_SIGN (0x05054b50)
	uint16_t data_size;						// size of data
	uint8_t data[1];						// signature data (variable size)
} DIGITAL_SIGNATURE;

typedef struct _END_OF_CDIR_RECORD {
	uint32_t signature;						// end of central dir signature END_CDIRECTORY_RECORD_SIGN (0x06054b50)
	uint16_t disk_num;						// number of this disk
	uint16_t cdir_disk_num;					// number of the disk with the start of the central directory
	uint16_t cdir_entries_count;			// total number of entries in the central directory on this disk				
	uint16_t cdir_entries_total_count;		// total number of entries in the central directory
	uint32_t cdir_size;						// size of the central directory
	uint32_t cdir_offset;					// offset of start of central directory with respect to the starting disk number
	uint16_t comment_len;					// .ZIP file comment length
	uint8_t comment[1];						// .ZIP file comment 
} END_OF_CDIR_RECORD;

typedef struct _ZIP64_END_OF_CDIR_RECORD {
	uint32_t signature;						
	uint64_t record_size;					// size of zip64 end of central directory record
	uint16_t ver_made;						// version made by
	uint16_t ver_needed;					// version needed to extract
	uint32_t disk_num;
	uint32_t cdir_disk_num;
	uint64_t cdir_entries_count;			// total number of entries in the central directory on this disk
	uint64_t cdir_entries_total_count;		// total number of entries in the central directory
	uint64_t cdir_size;
	uint64_t cdir_offset;

	//
	// zip64_extensible_data_sector[variable size]
	//

} ZIP64_END_OF_CDIR;

#pragma pack(pop)

struct ZipRecParameters {
	std::string file_path;
	std::string out_dir;
	uint64_t offset;
	std::string password;
	bool force_utf8;
	
	void Clear(void) {
		file_path = "";
		out_dir = "";
		offset = 0;
		password = "";
		force_utf8 = false;
	}
};

//													 
//ZipRecParameters() {								 
//	supported_args.emplace(std::make_pair("file", "Input file path"));
//	supported_args.emplace(std::make_pair("out_dir", "Output directory for result"));
//	supported_args.emplace(std::make_pair("offset", "Input file offset"));
//	supported_args.emplace(std::make_pair("force_utf8", ""));
//	supported_args.emplace(std::make_pair("pwd", "Password"));
//}

int StartRecovery(ZipRecParameters &param);
int ZipRec_Main(int argc, _TCHAR* argv[]);
int TestZipRec(void);
