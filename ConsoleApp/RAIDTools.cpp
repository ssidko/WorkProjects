//#include "RAIDTools.h"
//#include <iostream>
//
//#define MENU_XOR_CHECKER				(DWORD)1
//#define MENU_SLICER						(DWORD)2
//#define MENU_JPEG_ASSEMBLER				(DWORD)3
//#define MENU_QUIT						(DWORD)4
//
//#define BUFFER							BYTE * 
//#define SECTOR_SIZE						512
//
//typedef PhysicalDrive *PPHYSICALDRIVE;
//
//DWORD raid_members_count = 0;
//PPHYSICALDRIVE *drives = NULL;
//DWORD max_LBA_size;
//DWORD sign_LBA;
//TCHAR *work_dir;
//
//BOOL IsZeroFill(BYTE *buffer, DWORD count)
//{
//	for (DWORD i = 0; i < count; i++)
//	{
//		if (buffer[i] != 0x00)
//			return FALSE;
//	}
//	return TRUE;
//}
//
//BOOL InitializeRAID(void)
//{
//	DWORD drives_count = 0;
//	DWORD drive_number = 0;
//
//	if (drives)
//	{
//		//ClearRAID();
//	}
//
//	while (!drives_count)
//	{
//		std::cout << "RAID members count: ";
//		std::cin >> drives_count;
//		std::cout << endl;
//	}
//
//	drives = new PPHYSICALDRIVE[drives_count];
//
//	std::cout << "Enter PhysicalDrive numbers\n";
//	for (DWORD i = 0; i < drives_count; i++)
//	{
//		std::cout << " #" << i << ": ";
//		std::cin >> drive_number;
//
//		drives[i] = (PPHYSICALDRIVE)new PhysicalDrive(drive_number);
//		if (drives[i]->Open())
//		{
//			raid_members_count++;
//		}
//		else
//		{
//			std::cout << "	Error opening drive!\n\n";
//			return FALSE;
//		}
//	}
//
//	std::cout << endl;
//
//	return TRUE;
//}
//
//BOOL InitializeWorkDir(void)
//{
//	TCHAR dir[MAX_PATH] = {0};
//	std::cout << "Work directory: ";
//	std::cin >> dir;
//	return TRUE;
//}
//
//void ClearRAID()
//{
//	for (DWORD i = 0; i < raid_members_count; i++)
//	{
//		delete drives[i];
//	}
//	delete[] drives;
//	drives = NULL;
//	raid_members_count = 0;
//}
//
//BOOL StartXorCheck(void)
//{
//	LONGLONG lba = 0;
//	BYTE tmp[SECTOR_SIZE] = {0};
//	BUFFER *buffers = new BUFFER[raid_members_count];
//	for (DWORD i = 0; i < raid_members_count; i++)
//	{
//		buffers[i] = new BYTE[SECTOR_SIZE];
//	}
//
//	std::cout << endl << "Start process:\n";
//	while(TRUE)
//	{
//		for (DWORD i = 0; i < raid_members_count; i++)
//		{
//			if (SECTOR_SIZE != drives[i]->Read(buffers[i], SECTOR_SIZE))
//				return FALSE;
//		}
//		
//		for (DWORD i = 0; i < raid_members_count; i++)
//		{
//			for (DWORD j = 0; j < SECTOR_SIZE; j++)
//			{
//				tmp[j] ^= (buffers[i])[j];
//				int x = 0;
//			}
//		}
//
//		std::cout << "LBA " << lba << ": ";
//
//		if (IsZeroFill(tmp, SECTOR_SIZE))
//		{
//			cout << "XOR OK\n";
//		}
//		else
//		{
//			cout << "XOR ERROR\n";
//			_tprintf(_T("                     For resume press any key ...\n"));
//			_getch();
//		}
//		
//		lba++;
//		memset(tmp, 0x00, SECTOR_SIZE);
//	}
//
//	delete[] buffers;
//}
//
//int XorCheckerMain(void)
//{
//	system("cls");
//	std::cout << "=========================" << endl;
//	std::cout << "	XOR Checker" << endl;
//	std::cout << "=========================" << endl;
//
//	if (InitializeRAID())
//	{
//		StartXorCheck();
//	}
//	ClearRAID();
//	return 0;
//}
//
//void StartSlicer()
//{
//
//}
//
//int SlicerMain(void)
//{
//	system("cls");
//	std::cout << "=========================" << endl;
//	std::cout << "	Slicer" << endl;
//	std::cout << "=========================" << endl;
//	
//	if (InitializeRAID())
//	{
//		StartSlicer();
//	}
//	ClearRAID();
//	return 0;
//}
//
//int StartJpegAssembler(PPHYSICALDRIVE drives, DWORD member_count)
//{
//	return 0;
//}
//
//int JpegAssemblerMain()
//{
//	system("cls");
//	std::cout << "=========================" << endl;
//	std::cout << "	JPEG Assembler" << endl;
//	std::cout << "=========================" << endl;
//
//	if (InitializeRAID())
//	{
//		//StartJpegAssembler(drives, raid_members_count);
//	}
//	ClearRAID();
//	return 0;
//}
//
//int RAIDToolsMain(int argc, _TCHAR* argv[])
//{
//	DWORD choice = 0;
//	while (!choice)
//	{
//		system("cls");
//		std::cout << "=========================" << endl;
//		std::cout << "	RAID Tools" << endl;
//		std::cout << "=========================" << endl;
//		std::cout << "1. XOR checker" << endl;
//		std::cout << "2. Slicer" << endl;
//		std::cout << "3. JPEG Assembler" << endl;
//		std::cout << "4. Quit" << endl;
//		std::cout << "=========================" << endl;
//		std::cout << "Select: ";
//		std::cin >> choice;
//
//		switch (choice)
//		{
//		case MENU_XOR_CHECKER:
//			XorCheckerMain();
//			choice = 0;
//			break;
//		case MENU_SLICER:
//			SlicerMain();
//			choice = 0;
//			break;
//		case MENU_JPEG_ASSEMBLER:
//			SlicerMain();
//			choice = 0;
//			break;
//		case MENU_QUIT:
//			return 0;
//		default:
//			break;
//		}
//	}
//	return 0;
//}
