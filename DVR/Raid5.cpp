#include "Raid5.h"

Raid5::~Raid5()
{
	Close();
}

BOOL Raid5::Open(void)
{
	MyLib::WinFile *dev = nullptr;
	devs.resize(dev_names.size(), nullptr);
	for (int i = 0; i < dev_names.size(); i++) {
		dev = new MyLib::WinFile(dev_names[i].data(), MyLib::FileMode::kReadWrite);
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
