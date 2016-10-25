#include "G2fdbVolume.h"

using namespace G2FDB;

G2fdbVolume::G2fdbVolume(const std::string &volume_file) : io(volume_file, 256*512)
{
}

G2fdbVolume::~G2fdbVolume()
{
}

bool G2fdbVolume::Open(void)
{
	return io.Open();
}

bool G2fdbVolume::SetPointer(LONGLONG &offset)
{
	return io.SetPointer(offset);
}