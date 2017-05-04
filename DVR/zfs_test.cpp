#include "zfs_test.h"



void zfs_test(void)
{
	W32Lib::FileEx io;

	if (!io.Open("D:\\zol-0.6.1\\vdev0")) {
		return;
	}


	unsigned long long offset = VDEV_DATA_OFFSET;

	io.SetPointer(400*512);

	std::vector<unsigned char> buff(512, 0);
	if (io.Read(buff.data(), buff.size())) {
		uberblock *ub = (uberblock *)buff.data();

		int x = 0;
		
	}

	uberblock *ub = nullptr;

}
