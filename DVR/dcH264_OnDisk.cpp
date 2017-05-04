#include "dcH264_OnDisk.h"

dcH264::Volume::Volume(std::string &file_name) : io(file_name)
{
}

dcH264::Volume::~Volume()
{
	io.Close();
}

bool dcH264::Volume::Open(void)
{
	return io.Open();
}

void dcH264::Volume::Close(void)
{
	io.Close();
}

void dcH264::Volume::SetPointer(const LONGLONG &new_pointer)
{
	io.SetPointer(new_pointer);
}

LONGLONG dcH264::Volume::Pointer(void)
{
	return io.Pointer();
}

bool dcH264::Volume::ReadFrame(dvr::Frame &frame)
{
	return false;
}