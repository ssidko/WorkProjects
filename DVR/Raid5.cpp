#include "Raid5.h"
//#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////
//								Layout
//

Layout::Layout(Raid5LayoutType layout_type, DWORD device_count) : type(layout_type), devs_count(device_count)
{
	Initialize();
}

Layout::~Layout(void)
{
}

void Layout::Initialize(void)
{
	switch (type) {
	case kLeftAsymmetric:
		InitLeftAsymmetricLayout();
	}
}

void Layout::InitLeftAsymmetricLayout(void)
{
	int rows = devs_count;
	int columns = devs_count;

	table.resize((devs_count - 1) * devs_count, kXorBlock);

	int block = 0;
	for (int row = 0; row < rows; row++) {
		for (int column = 0; column < columns; column++) {
			if (column == ((devs_count - 1) - row)) {
				continue;
			}
			else {
				table[block++] = column;
			}
		}
	}
}

void Layout::PrintLayout(WinConsole &con)
{
	int rows = devs_count;
	int columns = devs_count - 1;

	int block = 0;
	for (int row = 0; row < rows; row++) {
		for (int column = 0; column < columns; column++) {
			con << table[block] << _T("\t");
			block++;
		}
		con << NEW_LINE;
	}

}

int Layout::operator[](int block_index)
{
	assert(block_index <= table.size());
	return table[block_index];
}


////////////////////////////////////////////////////////////////////////////////////////
//								Raid5
//

Raid5::Raid5(const std::vector<std::basic_string<TCHAR>> &device_names, MyLib::FileMode file_mode, const int raid_block_size, Raid5LayoutType layout_type) :
	dev_names(device_names),
	mode(file_mode),
	block_size(raid_block_size),
	layout(layout_type, device_names.size())
{

}

Raid5::~Raid5()
{
	Close();
}

BOOL Raid5::Open(void)
{
	MyLib::WinFile *dev = nullptr;
	devs.resize(dev_names.size(), nullptr);
	for (int i = 0; i < dev_names.size(); i++) {
		dev = new MyLib::WinFile(dev_names[i].data(), mode);
		if (dev) {
			if (dev->Open()) {
				devs[i] = dev;
				continue;
			}			
		}	
		return FALSE;
	}
	return TRUE;
}

void Raid5::Close(void)
{
	for (int i = 0; i < devs.size(); i++) {
		if (devs[i] != nullptr) {
			delete devs[i];
			devs[i] = nullptr;
		}
	}
	devs.clear();
}

BOOL Raid5::ReadBlock(const LONGLONG &block_number, BYTE *buffer)
{
	MyLib::WinFile *device = devs[ layout[block_number % layout.Size()] ];
	assert(device);
	LONGLONG offset = (block_number / (DeviceCount() - 1)) * block_size;
	if (device->SetPointer(offset)) {
		return device->Read(buffer, block_size);
	}
	return FALSE;
}

BOOL Raid5::WriteBlock(const LONGLONG &block_number, BYTE *buffer)
{
	MyLib::WinFile *device = devs[ layout[block_number % layout.Size()] ];
	assert(device);
	LONGLONG offset = (block_number / (DeviceCount() - 1)) * block_size;
	if (device->SetPointer(offset)) {
		return device->Write(buffer, block_size);		
	}
	return FALSE;
}
