#include "G2fdbVolume.h"
#include <cassert>

using namespace G2FDB;

void G2FDB::Frame::Clear(void)
{
	offset = 0;
	camera = 0;
	time.Clear();
	data.clear();
}

DWORD G2FDB::Frame::HeaderSize(void)
{
	return FRAME_HEADER_SIZE;
}

DWORD G2FDB::Frame::FullSize(void)
{
	return (data.size() + FRAME_HEADER_SIZE);
}

void G2FDB::FrameSequence::Clear(void)
{
	offset = 0;
	camera = 0;
	start_time.Clear();
	end_time.Clear();
	data.clear();
}

G2FDB::G2fdbVolume::G2fdbVolume(const std::string &volume_file) : io(volume_file, 256*512)
{
}

G2FDB::G2fdbVolume::~G2fdbVolume()
{
}

bool G2FDB::G2fdbVolume::Open(void)
{
	return io.Open();
}

bool G2FDB::G2fdbVolume::SetPointer(LONGLONG &offset)
{
	return io.SetPointer(offset);
}

bool G2FDB::G2fdbVolume::IsValidFrameHeader(const FRAME_HEADER &header)
{
	if (header.signature != FRAME_HEADER_SIGNATURE_1) {
		return false;
	}

	if (header.signature_2 != FRAME_HEADER_SIGNATURE_2) {
		return false;
	}

	if (0x00 != std::memcmp(header.string, "CAM", 3)) {
		return false;
	}

	if (header.str_length > sizeof(FRAME_HEADER::string)) {
		return false;
	}

	assert(header.data_size <= 64 * 1024);

	return true;
}

bool G2FDB::G2fdbVolume::FindNextFrame(LONGLONG &offset)
{
	LONGLONG offs = 0;
	FRAME_HEADER header = { 0 };
	BYTE signature[] = { 0x43, 0x41, 0x4D }; // 'CAM'

	while (io.Find(signature, sizeof(signature), offs)) {
		
		io.SetPointer(offs - 38);
		if (FRAME_HEADER_SIZE == io.Read(&header, sizeof(FRAME_HEADER))) {
			if (IsValidFrameHeader(header)) {
				offset = (offs - 38);
				return io.SetPointer(offset);			
			}		
		}
	}

	return false;
}

bool G2FDB::G2fdbVolume::ReadFrame(Frame &frame)
{
	FRAME_HEADER *header = nullptr;
	LONGLONG offset = io.Pointer();

	frame.data.resize(FRAME_HEADER_SIZE);

	if (FRAME_HEADER_SIZE == io.Read(&frame.data[0], sizeof(FRAME_HEADER))) {
		header = (FRAME_HEADER *)&frame.data[0];
		if (IsValidFrameHeader(*header)) {
			
			frame.data.resize(FRAME_HEADER_SIZE + header->data_size);
			if (header->data_size == io.Read(&frame.data[FRAME_HEADER_SIZE], header->data_size)) {
				
				frame.offset = offset;
				frame.camera = header->camera + 1;
				header->time.Timestamp(frame.time);

				return true;
			}
		}	
	}

	io.SetPointer(offset);
	frame.Clear();

	return false;
}

bool G2FDB::G2fdbVolume::ReadFrameSequence(FrameSequence &sequence)
{
	LONGLONG sequence_offset = 0;
	Frame frame;

	while(FindNextFrame(sequence_offset)) {	
	
		while(ReadFrame(frame)) {
		
		
		
		}
	
	}


	return false;
}
