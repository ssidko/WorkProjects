#include "stdafx.h"
#include "Calc.h"
#include "PlaChecker.h"
#include "stdafx.h"
#include <conio.h>
#include "smart.h"
#include "RAIDTools.h"
#include "PassHack.h"
#include "VMFSMain.h"
#include "cut_txt_main.h"
#include <iostream>
#include <windows.h>
#include "ZipRec.h"
#include "tibun.h"
#include "mp4_main.h"
#include "mover.h"
#include "sqliter.h"

#include "vmfs.h"

#include "h264.h"


BOOL IsValidString(BYTE *str, DWORD len)
{
	for (DWORD i = 0; i < len-1; i++)
	{
		if (str[i] == str[i+1])
			return FALSE;
	}
	return TRUE;
}

BOOL FindByteString(BYTE *buff, DWORD buff_sz, BYTE *str, DWORD str_len, DWORD &offset)
{
	for (DWORD i = 0; i <= (buff_sz - str_len); i++)
	{
		DWORD len = 0;
		for (DWORD j = 0; j < str_len; j++)
		{
			if (buff[i+j] != str[j])
				break;
			else
				len++;
		}
		if (len == str_len)
		{
			offset = i;
			return TRUE;
		}
	}
	return FALSE;
}

#define MAX_LBA								(LONGLONG)0x200000
#define MAX_BLOCK_NUMBER					(LONGLONG)0x4000
#define RAW_BLOCK_SIZE						(DWORD)4304*128
#define PAGE_MARKER							(DWORD)0xFFFF40FF
#define PAGES_PER_BLOCK						(DWORD)128

typedef struct _PAGE {
	BYTE data[4096];
	DWORD lba;
	DWORD flag;
	DWORD marker;
	BYTE dummy[196];
} PAGE;

typedef struct _BLOCK {
	PAGE page[PAGES_PER_BLOCK];
} BLOCK;

BOOL IsValidBlock(BLOCK *block)
{
	DWORD marker_counter = 0;
	PAGE *page = (PAGE *)block;

	for (DWORD i = 0 ; i < 128; i++) {
		if (page->marker == PAGE_MARKER) {
			marker_counter++;
		}
		page++;
	}

	return (marker_counter >= 120);
}


DWORD IPhone_CreateImageFromDumps(void)
{
	FileEx *dumps[2] = {0};

	FileEx dump0(_T("D:\\Work\\30065\\01_01.dump"));
	if (!dump0.Open()) return 0; dumps[0] = &dump0;
	FileEx dump1(_T("D:\\Work\\30065\\01_02.dump"));
	if (!dump1.Open()) return 0; dumps[1] = &dump1;

	FileEx sa0(_T("D:\\Work\\30065\\iPhone_0.sa"));
	if (!sa0.Create()) return 0;
	FileEx sa1(_T("D:\\Work\\30065\\iPhone_1.sa"));
	if (!sa1.Create()) return 0;

	FileEx image(_T("D:\\Work\\30065\\image.img"));
	if (!image.Create()) return 0;

	DWORD rw = 0;

	//PAGE pg;
	//memset(&pg, 0x00, sizeof(PAGE));

	//for (DWORD i = 0; i < 2; i++)
	//{
	//	FileEx *dump = dumps[i];
	//	dump->SetPointer(0);
	//	
	//	while (sizeof(PAGE) == dump->Read(&pg, sizeof(PAGE)))
	//	{
	//		if (i == 0) {
	//			sa0.Write((BYTE *)&pg.lba, 16);
	//		} else {
	//			sa1.Write((BYTE *)&pg.lba, 16);
	//		}
	//	}
	//}

	PAGE *page = NULL;
	DWORD prev_lba = 0;
	BLOCK *block = new BLOCK();
	memset(block, 0x00, sizeof(BLOCK));

	for (DWORD i = 0; i < 2; i++)
	{
		FileEx *dump = dumps[i];
		dump->SetPointer(0);
		while ( dump->Read(block, sizeof(BLOCK)) )
		{

			//
			// сборка блока в обратном порядке
			//

			//page = (PAGE *)block;
			//page += 127;
			//for (DWORD i = 0; i < 128; i++)
			//{
			//	if ((page->marker == PAGE_MARKER) && (page->lba < MAX_LBA))
			//	{
			//		image.SetPointer((LONGLONG)page->lba * 4096);
			//		image.Write(page, 4096);
			//	}
			//	page--;
			//}

			//
			// сборка блока в прямом порядке
			//
			page = (PAGE *)block;
			for (DWORD i = 0; i < 128; i++)
			{
				if ((page->marker == PAGE_MARKER) && (page->lba < MAX_LBA))
				{
					image.SetPointer((LONGLONG)page->lba * 4096);
					image.Write(page, 4096);
				}
				page++;
			}
		}		
	}

	delete block;

	return 0;
}

#define OFFSET			(LONGLONG)512*2112264
#define BLOCK_SZ		(LONGLONG)4096
#define BPG				(LONGLONG)32768

typedef struct _group_desc
{
	DWORD 	block_bitmap;					// Blocks bitmap block
	DWORD	inode_bitmap;					// Inodes bitmap block
	DWORD	inode_table;					// Inodes table block
	WORD	free_blocks_count;				// Free blocks count
	WORD	free_inodes_count;				// Free inodes count
	WORD	used_dirs_count;				// Directories count
	WORD	flags;
	DWORD	reserved[3];
} group_desc;

//
// Structure of the super block
//
typedef struct _super_block
{
	DWORD	s_inodes_count;					// Inodes count
	DWORD	s_blocks_count;					// Blocks count
	DWORD	s_r_blocks_count;				// Reserved blocks count
	DWORD	s_free_blocks_count;			// Free blocks count
	DWORD	s_free_inodes_count;			// Free inodes count
	DWORD	s_first_data_block;				// First Data Block
	DWORD	s_block_size;					// Block size
	LONG	s_fragment_size;				// Fragment size 
	DWORD	s_blocks_per_group;				// # Blocks per group
	DWORD	s_frags_per_group;				// # Fragments per group
	DWORD	s_inodes_per_group;				// # Inodes per group
	DWORD	s_mtime;						// Mount time
	DWORD	s_wtime;						// Write time
	WORD	s_mnt_count;					// Mount count
	SHORT	s_max_mnt_count;				// Maximal mount count
	WORD	s_magic;						// Magic signature EXT2_SUPER_MAGIC
	WORD	s_state;						// File system state
	WORD	s_errors;						// Behaviour when detecting errors
	WORD	s_minor_rev_level;				// minor revision level
	DWORD	s_lastcheck;					// time of last check
	DWORD	s_checkinterval;				// max. time between checks
	DWORD	s_creator_os;					// OS
	DWORD	s_rev_level;					// Revision level
	WORD	s_def_resuid;					// Default uid for reserved blocks
	WORD	s_def_resgid;					// Default gid for reserved blocks
	DWORD	s_first_ino;					// First non-reserved inode 
	WORD	s_inode_size;					// size of inode structure
	WORD	s_block_group_nr;				// block group # of this superblock 
	DWORD	s_feature_compat;				// compatible feature set
	DWORD	s_feature_incompat;				// incompatible feature set
	DWORD	s_feature_ro_compat;			// readonly-compatible feature set
	BYTE	s_uuid[16];						// 128-bit uuid for volume
	char	s_volume_name[16];				// volume name
	char	s_last_mounted[64];				// directory where last mounted
	DWORD	s_algorithm_usage_bitmap;		// For compression

	//
	// Performance hints.  Directory preallocation should only
	// happen if the EXT2_FEATURE_COMPAT_DIR_PREALLOC flag is on.
	//
	BYTE	s_prealloc_blocks;				// Nr of blocks to try to preallocate
	BYTE	s_prealloc_dir_blocks;			// Nr to preallocate for dirs
	WORD	s_reserved_gdt_blocks;			// Per group table for online growth

	//
	// Journaling support valid if EXT2_FEATURE_COMPAT_HAS_JOURNAL set.
	//
	BYTE	s_journal_uuid[16];				// uuid of journal superblock
	DWORD	s_journal_inum;					// inode number of journal file
	DWORD	s_journal_dev;					// device number of journal file
	DWORD	s_last_orphan;					// start of list of inodes to delete
	DWORD	s_hash_seed[4];					// HTREE hash seed
	BYTE	s_def_hash_version;				// Default hash version to use
	BYTE	s_jnl_backup_type;				// Default type of journal backup
	WORD	s_desc_size;					// Group desc. size: INCOMPAT_64BIT
	DWORD	s_default_mount_opts;
	DWORD	s_first_meta_bg;				// First metablock group
	DWORD	s_mkfs_time;					// When the filesystem was created
	DWORD	s_jnl_blocks[17];				// Backup of the journal inode
	DWORD	s_blocks_count_hi;				// Blocks count high 32bits
	DWORD	s_r_blocks_count_hi;			// Reserved blocks count high 32 bits
	DWORD	s_free_blocks_hi;				// Free blocks count
	WORD	s_min_extra_isize;				// All inodes have at least # bytes
	WORD	s_want_extra_isize;				// New inodes should reserve # bytes
	DWORD	s_flags;						// Miscellaneous flags
	WORD	s_raid_stride;					// RAID stride
	WORD	s_mmp_interval;					// # seconds to wait in MMP checking
	ULONGLONG s_mmp_block;					// Block for multi-mount protection
	DWORD   s_raid_stripe_width;			// blocks on all data disks (N*stride)
	BYTE	s_log_groups_per_flex;			// FLEX_BG group size
	BYTE	s_reserved_char_pad;
	WORD	s_reserved_pad;					// Padding to next 32bits
	ULONGLONG s_kbytes_written;				// nr of lifetime kilobytes written
	DWORD   s_reserved[160];				// Padding to the end of the block
} super_block;
#pragma pack()

int CreateDescriptorTable(void)
{
	PhysicalDrive dump(1);
	if (!dump.Open()) return -1;

	FileEx descr(_T("F:\\descr.bin"));
	if (!descr.Create()) return -1;

	super_block *psb = NULL;

	BYTE sector[512] = {0};

	for (LONGLONG i = 0; i < 806; i++)
	{
		if (dump.SetPointer(OFFSET + i*BPG*BLOCK_SZ))
		{
			memset(sector, 0x00, sizeof(sector));
			dump.Read(sector, sizeof(sector));
			psb = (super_block *)sector;
			if(psb->s_magic == 0xEF53)
			{
				group_desc gd;
				memset(&gd, 0x00, sizeof(group_desc));
				gd.block_bitmap = (DWORD)i*BPG + 8;
				gd.inode_bitmap = (DWORD)i*BPG + 8 + 1;
				gd.inode_table = (DWORD)i*BPG + 8 + 2;

				descr.Write(&gd, sizeof(group_desc));
			}
			else
			{
				group_desc gd;
				memset(&gd, 0x00, sizeof(group_desc));
				gd.block_bitmap = (DWORD)i*BPG;
				gd.inode_bitmap = (DWORD)i*BPG + 1;
				gd.inode_table = (DWORD)i*BPG + 2;

				descr.Write(&gd, sizeof(group_desc));
			}
		}
	}
	return 0;
}

int sql_main(void);

#include "sc2calc.h"

////////////////////////////////////////////////////////////////////////////////////////////
//========================================================================================//
///*

#define HDR_MAGIC					(DWORD)0x56414844 

#pragma pack (1)
typedef struct _HDR {
	DWORD sign;				// HDR_MAGIC
	WORD flag;
	WORD cam_num;			// номер камеры
	DWORD index;
	DWORD size;
	DWORD date;
} HDR, PHDR;
#pragma pack ()

//*/
//========================================================================================//
////////////////////////////////////////////////////////////////////////////////////////////

class RaidCounter
{
	static DWORD number[];
	DWORD index;
public:
	RaidCounter(void)
	{
		index = 0;
	}

	DWORD Next()
	{
		DWORD ret = number[index];
		if (index >= 11) index = 0;
		else ++index;
		return ret;
	}
};

DWORD RaidCounter::number[] = {0, 1, 3, 4, 0, 2, 3, 5, 1, 2, 4, 5};

class OutStream
{
	File *file;
	LONGLONG base_offset;
	LONGLONG buffer_offset;
	BYTE *buffer;
	DWORD buffer_size;
	DWORD offset;
	DWORD readed;

public:
	OutStream(File *_file, LONGLONG &_offset) : 
	  file(_file),
		  base_offset(_offset),
		  buffer_offset(_offset),
		  buffer(NULL),
		  offset(0),
		  readed(0)
	  {
		  assert(file);
		  if(!file->IsOpen())
			  if (!file->Open())
				  throw 0;
		  if (!file->SetPointer(base_offset))
			  throw 0;

		  buffer_size = 256*SECTOR_SIZE;
		  buffer = new BYTE[buffer_size];
	  }

	  ~OutStream()
	  {
		  if (buffer) delete[] buffer;
	  }

	  DWORD Read(BYTE *buff, DWORD count)
	  {
		  DWORD cnt = 0;
		  DWORD out_buff_offset = 0;
		  while (count) {
			  cnt = min(count, (readed - offset));
			  if (cnt) {
				  memcpy(&buff[out_buff_offset], &buffer[offset], cnt);
				  offset += cnt;
				  out_buff_offset += cnt;
				  count -= cnt;
			  }
			  else {
				  offset = 0;
				  buffer_offset += readed;
				  if (!file->SetPointer(buffer_offset))
					  break;
				  if (!(readed = file->Read(buffer, buffer_size)))
					  break;
			  }
		  }
		  return out_buff_offset;
	  }

	  void BytesReaded(LONGLONG &readed)
	  {
		  readed = buffer_offset + offset;	
	  }
};

#define RAID_OFFSET						(DWORD)512*1024
#define RAID_BLOCK_SIZE					(DWORD)128*512
#define RAID_PD							(DWORD)0xFEFEFEFE
#define RAID_RS							(DWORD)0xF0F0F0F0
#define RAID_MISSING					(DWORD)0

class RaidOrder
{
	DWORD order[8];
	DWORD index;
public :
	RaidOrder() : index(0)
	{
		order[0] = 0;	
		order[1] = 1;
		order[2] = RAID_PD;
		order[3] = RAID_RS;
		order[4] = RAID_PD;
		order[5] = RAID_RS;
		order[6] = 2;
		order[7] = 3;
	}

	DWORD Next()
	{
		DWORD res = 0;
		if (index >= 8) index = 0;
		res = order[index];
		index++;
		return res;
	}
};

void XorBlock (DWORD *block_1, DWORD *block_2, DWORD *result_block, DWORD dw_count)
{
	for (register DWORD i = 0; i < dw_count; i++) {
		result_block[i] = block_1[i] ^ block_2[i];
	}
}

#include <ctime>

#pragma pack (push)
#pragma pack (1)

typedef struct _VHDR {
	DWORD sign : 24;
	DWORD type : 4;
	DWORD flag : 4;
} VHDR;

#pragma pack(pop)

void testing(void)
{
	FileEx file(_T("J:\\Work\\33275\\tst.h264"));
	if (file.Open()) {
		VHDR hdr;
		DWORD size = 0;
		DWORD unk = 0;
		DWORD time = 0;
		LONGLONG chunk_offset = 0;
		while (sizeof(VHDR) == file.Read(&hdr, sizeof(VHDR))) {
			if ((hdr.sign == 0x010000) && (hdr.flag == 0x0F)) {
				if (!file.GetPointer(&chunk_offset)) {
					break;
				}
				chunk_offset -= sizeof(VHDR); 
				if (hdr.type == 0x0D) {
					if (sizeof(DWORD) != file.Read(&size, sizeof(DWORD))) {
						break;
					}
					if (!file.SetPointer((LONGLONG)size, FILE_CURRENT)) {
						break;
					}
					continue;
				} else if (hdr.type == 0x0C) {					
					if (sizeof(DWORD) != file.Read(&unk, sizeof(DWORD))) {
						break;
					}
					if (sizeof(DWORD) != file.Read(&time, sizeof(DWORD))) {
						break;
					}					
					if (sizeof(DWORD) != file.Read(&size, sizeof(DWORD))) {
						break;
					}
					if (!file.SetPointer((LONGLONG)size, FILE_CURRENT)) {
						break;
					}
					continue;
				} else {
					// unknown type !!!
					int g = 0;
				}
			}
			int zz = 0;
		}
		int y = 0;
	}

}

#include "sqliter.h"

#include "mxf.h"

#include "dshowtest.h"

using namespace sqliter;

namespace RAID6
{

#define DRIVES_COUNT				15
#define RAID6_BLOCK_SIZE			(DWORD)256*512			

	enum {
		kDataBlock,
		kParityBlock,
		kRSBlock
	};
	
	//
	//	Rs	1	2	3	4	5	Pd
	//	6	7	8	9	10	Pd	Rs
	//	12	13	14	15	Pd	Rs	11
	//	18	19	20	Pd	Rs	16	17
	//	24	25	Pd	Rs	21	22	23
	//

	class DrivesOrder 
	{
	private:
		DWORD *numbers;
		DWORD count;
	public:
		DrivesOrder(DWORD drives_count) : count(drives_count)
		{
			numbers = new DWORD[drives_count];
			memset(numbers, kDataBlock, count*sizeof(DWORD));
			numbers[0] = kRSBlock;
			numbers[count-1] = kParityBlock;			
		}

		~DrivesOrder() 
		{
			delete[] numbers;
		}

		DWORD DriveIndex(ULONGLONG logical_block_number)
		{
			ULONGLONG drive_block_number = logical_block_number / (count - 2);
			DWORD row = drive_block_number % count;
			DWORD column = logical_block_number % (count - 2);

			DWORD counter = 0;
			DWORD drive_number = column;
			DWORD index = (MakeRow(row) + 1);

			while (true) {
				if (index >= count) {
					index = 0;
					continue;
				}				
				if (counter >= drive_number) {
					return index;
				}
				if (numbers[index] == kDataBlock) {
					counter++;
					index++;
					continue;
				}
				else {
					index++;
					continue;
				}
			}
		}

		DWORD MakeRow(DWORD row)
		{
			memset(numbers, kDataBlock, count*sizeof(DWORD));
			DWORD rs_index = (row == 0 ? 0 : (count - row)); // RS block
			DWORD pd_index = (count - 1) - row;
			numbers[rs_index] = kRSBlock;
			numbers[pd_index] = kParityBlock;
			return rs_index;
		}
		
		void Test(void)
		{
			for (DWORD r = 0; r < count; r++) {
				MakeRow(r);
				for (DWORD c = 0; c < count; c++) {
					if (numbers[c] == kDataBlock)
						cout << "00 ";
					else if (numbers[c] ==kParityBlock)
						cout << "Pr ";
					else if (numbers[c] ==kRSBlock)
						cout << "RS ";
				}
				cout << "\n";
			}
		}
	};

	class Raid6
	{
	private:
		DrivesOrder order;
		vector<PhysicalDrive *> drives;
		DWORD block_size;
	public:
		Raid6(DWORD drives_numbers[], DWORD drives_count, DWORD raid_block_size) : order(drives_count), block_size(raid_block_size)
		{
			PhysicalDrive *drive = NULL;
			for (DWORD x = 0; x < drives_count; x++) {
				drive = new PhysicalDrive(drives_numbers[x]);
				drives.push_back(drive);
			}
		}

		BOOL Open(void)
		{
			for (DWORD x = 0; x < drives.size(); x++) {
				if (!drives[x]->Open()) {
					return FALSE;
				}
				else {
					cout << "Drive #" << x << " opened" << "\n";
				}
			}
			return TRUE;
		}

		BOOL ReadBlock(ULONGLONG &logical_block_number, BYTE *buffer)
		{	
			ULONGLONG drive_block_number = logical_block_number / (drives.size() - 2);
			PhysicalDrive *drive = drives[order.DriveIndex(logical_block_number)];
			drive->SetPointer((LONGLONG)(drive_block_number*RAID6_BLOCK_SIZE));
			if ( drive->Read(buffer, RAID6_BLOCK_SIZE) == RAID6_BLOCK_SIZE ) {
				return TRUE;
			}
			else {
				cout << "Error read drive :" << ::GetLastError() << "\n";
				return FALSE;
			}
		}
	};

	BOOL ConcatenateFiles(File &dst_file, LONGLONG &dst_offset, File &src_file, LONGLONG &src_offset) 
	{
		DWORD rw = 0;
		const int buff_size = 256*512;
		BYTE *buff = new BYTE[buff_size];
		
		if(!dst_file.IsOpen()) if(!dst_file.Open()) return FALSE;
		if(!src_file.IsOpen()) if(!src_file.Open()) return FALSE;

		dst_file.SetPointer(dst_offset);
		src_file.SetPointer(src_offset);
		if ((512 != dst_file.Read(&buff[0], 512)) || (512 != src_file.Read(&buff[512], 512))) return FALSE;
		if (memcmp(&buff[0], &buff[512], 512)) return FALSE;

		dst_file.SetPointer(dst_offset);
		src_file.SetPointer(src_offset);
		while (rw = src_file.Read(buff, buff_size)) {
			dst_file.Write(buff, rw);
		}
		return TRUE;
	}


	int main(void)
	{
		//DWORD err = 0;
		//DWORD drives_numbers[DRIVES_COUNT] = {8,16,3,14,15,10,2,6,9,5,12,7,13,1,11};
		//ULONGLONG start_block = /*67108862*//*70464620*/71543383;
		//Raid6 raid(drives_numbers, DRIVES_COUNT, RAID6_BLOCK_SIZE);
		//if (raid.Open()) {
		//	cout << "RAID opened\n";
		//	File out_file(_T("Y:\\new\\tail.bin"));
		//	if (out_file.Create()) {
		//		cout << "Out file created\n";
		//		BYTE *buff = new BYTE[RAID6_BLOCK_SIZE];
		//		while (raid.ReadBlock(start_block, buff)) {
		//			out_file.Write(buff, RAID6_BLOCK_SIZE);
		//			start_block++;
		//		}
		//	}
		//	else {
		//		cout << "Error craete file: " << ::GetLastError() << "\n";
		//	}
		//}

		File dst_file(_T("G:\\35116\\Virtual Block RAID 1.dsk"));
		//File src_file(_T("G:\\35116\\new\\tail-70464620.bin"));
		//File src_file(_T("G:\\35116\\18315106048-sec.dsk"));
		File src_file(_T("G:\\35116\\new\\tail-67108862.bin"));
		LONGLONG dst_offset = (LONGLONG)(67108862LL*RAID6_BLOCK_SIZE);
		LONGLONG src_offset = 0;

		ConcatenateFiles(dst_file, dst_offset, src_file, src_offset); 


		return 0;
	}
}

namespace DHFS
{
#pragma push()
#pragma pack(1)
	typedef struct _TIME_STAMP {
		DWORD sec:6;
		DWORD min:6;
		DWORD hour:5;
		DWORD day:5;
		DWORD month:4;
		DWORD year:6;
	} TIME_STAMP;
#pragma pop()

#define FRAME_HEADER_MAGIC			'DHAV'

	typedef struct _FRAME_HEADER {
		DWORD magic;
		WORD flags;
		WORD camera;
		DWORD counter;
		DWORD size;
		TIME_STAMP time;
		BYTE unk1[8];
		DWORD unk2;
		BYTE data[1];
	} FRAME_HEADER;

#define FRAME_FOOTER_MAGIC			'dhav'

	typedef struct _FRAME_FOOTER {
		DWORD magic;
		DWORD size;
	} FRAME_FOOTER;

}

#include "WinConsole.h"

using namespace DHFS;

int _tmain(int argc, _TCHAR* argv[])
{
	_tprintf(_T("Test message\n"));
	
	WinConsole console;
	console.Test();

	_tprintf(_T("\nPress any key for exit ..."));
	_getch();

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//												MySQL Testing
//////////////////////////////////////////////////////////////////////////////////////////////////////

//#include "mysql_connection.h"
//#include "mysql_driver.h"
//	
//#include <cppconn/driver.h>
//#include <cppconn/exception.h>
//#include <cppconn/resultset.h>
//#include <cppconn/statement.h>
//#include <cppconn/prepared_statement.h>

//int sql_main(void)
//{
//	try {
//		sql::Driver *driver = sql::mysql::get_driver_instance();
//
//		sql::SQLString name = driver->getName();
//		int mj_ver = driver->getMajorVersion();
//		int min_ver = driver->getMinorVersion();
//		int ptch_ver = driver->getPatchVersion();
//
//		sql::SQLString host = "ssidko4";
//		sql::SQLString user = "root";
//		sql::SQLString pwd = "19820903";
//
//		std::auto_ptr<sql::Connection> con(driver->connect(host, user, pwd));
//		std::auto_ptr<sql::Statement> stmt(con->createStatement());
//
//		std::auto_ptr<sql::ResultSet> res(stmt->executeQuery("SHOW DATABASES"));
//		size_t rows = res->rowsCount();
//		std::cout << "Number of rows: " << rows << endl;
//
//		sql::ResultSetMetaData *meta = res->getMetaData();
//
//		sql::SQLString col_name = meta->getColumnLabel(1);
//
//		std::cout << col_name << endl << endl;
//		while (res->next())
//		{
//			std::cout << res->getString(col_name) << endl;
//			int x = 0;
//		}
//
//		int x = 0;
//
//	} catch (sql::SQLException &e) {
//		/*
//		The MySQL Connector/C++ throws three different exceptions:
//
//		- sql::MethodNotImplementedException (derived from sql::SQLException)
//		- sql::InvalidArgumentException (derived from sql::SQLException)
//		- sql::SQLException (derived from std::runtime_error)
//		*/
//		cout << "# ERROR: SQLException in file:" << endl << __FILE__ << endl;
//		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
//		/* Use what() (derived from std::runtime_error) to fetch the error message */
//		cout << "# ERROR: " << e.what() << endl;
//		cout << " (MySQL error code: " << e.getErrorCode();
//		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
//		return EXIT_FAILURE;
//	}
//
//	return 0;
//}

