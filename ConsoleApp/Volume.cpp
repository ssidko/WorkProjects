#include "Volume.h"
#include <assert.h>


Volume::Volume(const std::string &io_file_name, const long long &offset_in_bytes, size_t io_block_size, const long long &blocks_count) :
	io(io_file_name.c_str()),
	volume_offset(offset_in_bytes),
	block_size(io_block_size)
{
	assert(io_block_size);
}


Volume::~Volume()
{
	io.Close();
}

bool Volume::Open(void)
{
	if (io.Open()) {
		if (io.SetPointer(volume_offset)) {
			return true;
		} else {
			io.Close();
		}	
	};
	return false;
}

void Volume::Close(void)
{
	return io.Close();
}

size_t Volume::BlockSize(void)
{
	return block_size;
}

long long Volume::BlocksCount(void)
{
	return blocks_count;
}

bool Volume::SetPointer(const long long &offset_in_blocks)
{
	return (io.SetPointer(volume_offset + offset_in_blocks) != 0x00);
}

long long Volume::Pointer(void)
{
	long long  offset = 0;
	if (io.GetPointer(&offset)) {
		return (offset - volume_offset);
	}
	return -1;
}

size_t Volume::ReadBlock(void *block_buffer, size_t blocks_count)
{
	assert(block_buffer);
	assert(blocks_count);
	return (size_t)io.Read(block_buffer, block_size*blocks_count);
}

size_t Volume::WriteBlock(void *block_buffer, size_t blocks_count)
{
	assert(block_buffer);
	assert(blocks_count);

	return (size_t)io.Write(block_buffer, block_size*blocks_count);
}
