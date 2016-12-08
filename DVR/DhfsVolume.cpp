#include "DhfsVolume.h"
#include "bitstream_reader.h"
#include "h264_ps.h"
#include <cassert>

#define DHFS_FRAME_MIN_SIZE			(DWORD)(sizeof(DHFS::FRAME_HEADER) + sizeof(DHFS::FRAME_FOOTER))
#define DHFS_FRAME_MAX_SIZE			(DWORD)(2*1024*1024)

void DHFS::FrameInfo::Clear(void)
{
	offset = 0;
	camera = 0;
	time.Clear();	
}

void DHFS::Frame::Clear(void)
{
	info.Clear();
	data.clear();
}

DHFS::FRAME_HEADER * DHFS::Frame::Header(void)
{
	return (FRAME_HEADER *)data.data();
}

size_t DHFS::Frame::Size(void)
{
	return data.size();
}

size_t DHFS::Frame::HeaderSize(void)
{
	DWORD *start_prefix = nullptr;
	switch (this->Header()->frame_type) {
	case 0xFC:
		start_prefix = (DWORD *)&data[0x20];
		if (*start_prefix == 0x01000000) {
			return 0x20;
		}
		start_prefix = (DWORD *)&data[0x24];
		if (*start_prefix == 0x01000000) {
			return 0x24;
		}
		assert(false);
		return 0;
	case 0xFD:
		start_prefix = (DWORD *)&data[0x28];
		if (*start_prefix == 0x01000000) {
			return 0x28;
		}
		start_prefix = (DWORD *)&data[0x2C];
		if (*start_prefix == 0x01000000) {
			return 0x2C;
		}
		assert(false);
		return 0;
	case 0xF1:
		return 0x28;
	case 0xF0:
		return 0x24;
	default:
		return 0;
	}
}

size_t DHFS::Frame::PayloadSize(void)
{
	return Size() - (HeaderSize() + sizeof(DHFS::FRAME_FOOTER)); 
}

void DHFS::FrameSequence::Clear(void)
{
	first_frame.Clear();
	last_frame.Clear();
	width = 0;
	height = 0;
	frames_count = 0;
	data.clear();	
}

void DHFS::FrameSequence::AddFrame(Frame &frame)
{
	last_frame = frame.info;
	frames_count++;

	size_t size = data.size();
	data.resize(size + frame.PayloadSize());
	std::memcpy(&data[size], &frame.data[frame.HeaderSize()], frame.PayloadSize());
}

void DHFS::FrameSequence::AddFirstFrame(Frame &frame)
{
	Clear();
	frames_count = 1;
	first_frame = frame.info;
	AddFrame(frame);
}

DHFS::DhfsVolume::DhfsVolume(const std::string &volume_file, const LONGLONG &size) : io(volume_file, 256*512, size)
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
		if (header->magic == FRAME_HEADER_MAGIC){
			if ((header->frame_size >= DHFS_FRAME_MIN_SIZE) && (header->frame_size < DHFS_FRAME_MAX_SIZE)) {

				frame.data.resize(header->frame_size);
				header = (FRAME_HEADER *)frame.data.data();
				footer = (FRAME_FOOTER *)&frame.data[frame.data.size() - sizeof(FRAME_FOOTER)];

				size_t tail_size = header->frame_size - sizeof(FRAME_HEADER);
				if (io.Read(&frame.data[sizeof(FRAME_HEADER)], tail_size) == tail_size) {
					if (footer->magic == FRAME_FOOTER_MAGIC && footer->frame_size == header->frame_size) {
						frame.info.offset = curr_offset;
						frame.info.camera = header->camera + 1;
						frame.info.time = header->time.Timestamp();
						return true;
					}
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

bool DHFS::DhfsVolume::FindAndReadFrameSequence(FrameSequence &sequence, size_t max_size)
{
	Frame frame;
	if (FindAndReadFrame(frame)) {
		sequence.AddFirstFrame(frame);
		while (ReadFrame(frame)) {

			assert(sequence.first_frame.camera == frame.info.camera);

			if ((frame.Header()->frame_type != 0xfd) && (frame.Header()->frame_type != 0xfc)) {
				continue;
			}

			if ((sequence.data.size() + frame.data.size()) >= max_size) {
				io.SetPointer(frame.info.offset);
				return true;
			}

			if (frame.Header()->frame_type == 0xfd) {
				size_t width = 0;
				size_t height = 0;
				if (GetWidthAndHeight(frame, width, height)) {
					if (sequence.width && sequence.height) {
						if ((sequence.width != width) || (sequence.height != height)) {
							io.SetPointer(frame.info.offset);
							return true;
						}						
					} else {
						sequence.width = width;
						sequence.height = height;
					}
				}
			}

			sequence.AddFrame(frame);

		}
		return true;
	}
	sequence.Clear();
	return false;
}

bool DHFS::GetWidthAndHeight(Frame &frame, size_t &width, size_t &height)
{
	SPS sps = { 0 };
	uint32_t *start_prefix = nullptr;
	size_t start_prefix_size = 4;
	size_t offset = frame.HeaderSize();

	while (frame.data.size() >= (offset + start_prefix_size)) {
		start_prefix = (uint32_t *)&frame.data[offset];
		if (*start_prefix == 0x01000000) {
		
			bitstream_reader bs(&frame.data[offset + start_prefix_size], frame.data.size() - (offset + start_prefix_size));

			int forbidden_zero_bit = bs.f(1);
			int nal_ref_idc = bs.u(2);
			int nal_unit_type = bs.u(5);

			if (nal_unit_type == 7) {
				h264_GetWidthHeight(bs, width, height);
				return (width && height);
			}		
		}
		offset++;
	}
	return false;
}
