#pragma once

#include <windows.h>
#include <vector>

#include <WinFile.h>
#include <WinConsole.h>

enum Raid5LayoutType {
	kUnknownLayout,
	kLeftAsymmetric,
	kLeftSymmetric,
	kRightAsymmetric,
	kRightSymmetric,
};

enum BlockType {
	kXorBlock = -1,
	kReedSolomonBlock = -2,
};

struct BlockPosition {
	DWORD device_number;
	LONGLONG block_number;
};

class Layout
{
private:
	int devs_count;
	Raid5LayoutType type;
	std::vector<int> table;
	void Initialize(void);
	void InitLeftAsymmetricLayout(void);
public:
	Layout(Raid5LayoutType layout_type, DWORD device_count);
	~Layout(void);
	void PrintLayout(WinConsole &con);
	int Size(void) { return table.size(); }
	int operator[](int block_index);
};

class Raid5
{
private:
	int block_size;
	std::vector<std::basic_string<TCHAR>> dev_names;
	std::vector<MyLib::WinFile *> devs;
	MyLib::FileMode mode;
	Layout layout;

public:
	Raid5(const std::vector<std::basic_string<TCHAR>> &device_names, MyLib::FileMode file_mode,  const int raid_block_size, Raid5LayoutType layout_type);
	~Raid5();
	BOOL Open(void);
	void Close(void);
	size_t DeviceCount(void) { return dev_names.size(); }
	BOOL ReadBlock(LONGLONG block_number, BYTE *buffer);
	BOOL WriteBlock(LONGLONG block_number, BYTE *buffer);
};

