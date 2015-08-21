#pragma once

#include <windows.h>
#include <vector>

#include <WinFile.h>

enum Raid5LayoutType {
	kUnknownLayout,
	kLeftAasymmetric,
	kLeftSymmetric,
	kRightAsymmetric,
	kRightSymmetric,
};

class Layout
{
private:
	int rows;
	int columns;

};

class Raid5
{
private:
	int block_size;
	std::vector< std::basic_string<TCHAR> > dev_names;
	std::vector<MyLib::WinFile *> devs;
public:
	Raid5(const std::vector< std::basic_string<TCHAR> > &device_names, const int raid_block_size, Raid5LayoutType layout_type) : dev_names(device_names), block_size(raid_block_size) {}
	~Raid5();
	BOOL Open(void);
	void Close(void);
	size_t DeviceCount(void) { return dev_names.size(); }
	BOOL ReadBlock(LONGLONG block_number, BYTE *buffer);
	BOOL WriteBlock(LONGLONG block_number, BYTE *buffer);
};

