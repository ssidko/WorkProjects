#include "DhfsVolume.h"

#define DHFS_FRAME_MAX_SIZE			(DWORD)2*1024*1024

void DHFS::Frame::Clear(void)
{
	offset = 0;
	data.clear();
}

DHFS::FRAME_HEADER * DHFS::Frame::Header(void)
{
	return (FRAME_HEADER *)data.data();
}

DHFS::DhfsVolume::DhfsVolume(const std::string &volume_file) : io(volume_file, 256*512)
{
}

DHFS::DhfsVolume::~DhfsVolume()
{
}

bool DHFS::DhfsVolume::Open(void)
{
	if (io.Open()) {
		io.SetPointer(0x22 * 512LL);
		size_t rd = io.Read(&volume_header, sizeof(volume_header));
		io.SetPointer(0LL);
		return true;
	}
	return false;
}

bool DHFS::DhfsVolume::SetPointer(LONGLONG offset)
{	
	return io.SetPointer(offset);
}

LONGLONG DHFS::DhfsVolume::Poiner(void)
{
	return io.Pointer();
}

bool DHFS::DhfsVolume::ReadFrame(Frame & frame)
{
	LONGLONG curr_offset = io.Pointer();

	FRAME_HEADER *header = nullptr;
	FRAME_FOOTER *footer = nullptr;

	frame.data.resize(sizeof(FRAME_HEADER));
	header = (FRAME_HEADER *)frame.data.data();
	if (io.Read(&frame.data[0], sizeof(FRAME_HEADER)) == sizeof(FRAME_HEADER)) {
		if ((header->magic == FRAME_HEADER_MAGIC) && (header->frame_size < DHFS_FRAME_MAX_SIZE)) {
		
			frame.data.resize(header->frame_size);
			header = (FRAME_HEADER *)frame.data.data();
			footer = (FRAME_FOOTER *)&frame.data[frame.data.size() - sizeof(FRAME_FOOTER)];

			size_t tail_size = header->frame_size - sizeof(FRAME_HEADER);
			if (io.Read(&frame.data[sizeof(FRAME_HEADER)], tail_size) == tail_size) {
				if (footer->magic == FRAME_FOOTER_MAGIC && footer->frame_size == header->frame_size) {
					frame.offset = curr_offset;
					return true;
				}			
			}
		}
	}

	frame.Clear();
	io.SetPointer(curr_offset);
	return false;
}

/*
* »щет валидный фрейм и считывает его.
* ”казатель устанавливаетс€ на позицию сразу же за фреймом,
* так что-бы последующим чтением можно было считать следущий за ним фрейм (если он там есть :))
*/
bool DHFS::DhfsVolume::FindAndReadFrame(Frame & frame)
{
	BYTE signature[] = { 'D', 'H', 'A', 'V' };
	LONGLONG offset = 0;

	while (io.Find(signature, sizeof(signature), offset)) {
		if (ReadFrame(frame)) {
			return true;
		}
		else {
			io.SetPointer(offset + 1);
		}
	}

	return false;
}
