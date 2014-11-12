#ifndef _PHYSICALDRIVE
#define _PHYSICALDRIVE

//#include "Global.h"
#include "File.h"

//#define _NTDDSTOR_H_
//#include "Other\ntddstor.h"
#include <WinIoCtl.h>
#include <stdio.h>

#define MAX_DRIVE_NAME								32
#define INVALID_PHYSICAL_DRIVE_NUMBER				(DWORD)(-1)


namespace W32Lib
{
	// typedef enum _MEDIA_TYPE in "WinIoCtl.h" 
	static const char *str_media_type[] =	{("Unknown"), ("F5_1Pt2_512"), ("F3_1Pt44_512"), ("F3_2Pt88_512"),
											("F3_20Pt8_512"), ("F3_720_512"), ("F5_360_512"), ("F5_320_512"),
											("F5_320_1024"), ("F5_180_512"), ("F5_160_512"), ("RemovableMedia"),
											("FixedMedia"), ("F3_120M_512"), ("F3_640_512"), ("F5_640_512"),
											("F5_720_512"), ("F3_1Pt2_512"), ("F3_1Pt23_1024"), ("F5_1Pt23_1024"),
											("F3_128Mb_512"), ("F3_230Mb_512"), ("F8_256_128"), ("F3_200Mb_512"),
											("F3_240M_512"), ("F3_32M_512")};

	static const char *str_bus_type[] =	{("Unknown"), ("Scsi"), ("Atapi"), ("Ata"), ("1394"), ("Ssa"),
											("Fibre"), ("Usb"), ("RAID"), ("iScsi"), ("Sas"), ("Sata"), 
											("Sd"), ("Mmc"), ("Max")};


	class PhysicalDrive : public File
	{
		private:
			DWORD number;



		public:
			static const char *MediaType2Str(MEDIA_TYPE media_type) {return W32Lib::str_media_type[media_type];} // done
			static const char *BusType2Str(STORAGE_BUS_TYPE bus_type) {return W32Lib::str_bus_type[bus_type];}

			PhysicalDrive(DWORD drive_number);
			PhysicalDrive(const char *drive_name);
			~PhysicalDrive(void);

			BOOL Open(const char *drive_name = NULL);
			BOOL Open(DWORD dwDesiredAccess, DWORD dwShareMode);
			DWORD GetPhysicalDriveNumber(void) {return number;}

			BOOL GetDeviceNumber(STORAGE_DEVICE_NUMBER *sdn); // done
			BOOL GetControllerNumber(DISK_CONTROLLER_NUMBER *dcn); // error!!!
			BOOL GetStorageDeviceDescriptor();	// in progress
			BOOL GetDriveGeometryEx(DISK_GEOMETRY_EX *disk_geometry_ex); // done

			// IOCTL_DISK_GET_DRIVE_GEOMETRY_EX
			BOOL IoctlDiskGetDriveGeometryEx(OUT PDISK_GEOMETRY_EX *geometry_ex,
											 OUT PDISK_PARTITION_INFO *partition_info,
											 OUT PDISK_DETECTION_INFO *detection_info);
	};
}

#endif

