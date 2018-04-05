#include "G2fdbVolume.h"
#include <cassert>

using namespace G2FDB;

void G2FDB::Frame::Clear(void)
{
	offset = 0;
	camera = 0;
	width = 0;
	height = 0;
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
	width = 0;
	height = 0;
	frames_count = 0;
	start_time.Clear();
	end_time.Clear();
	data.clear();
}

void G2FDB::FrameSequence::AddData(BYTE *frame_data, size_t size)
{
	size_t origin_size = data.size();
	data.resize(origin_size + size);
	std::memcpy(&data[origin_size], frame_data, size);
}

void G2FDB::FrameSequence::AddFirstFrame(Frame & first_frame)
{
	offset = first_frame.offset;
	camera = first_frame.camera;
	width = first_frame.width;
	height = first_frame.height;
	frames_count = 1;
	start_time = first_frame.time;
	end_time = first_frame.time;
	data.clear();
	AddData(&first_frame.data[FRAME_HEADER_SIZE], first_frame.data.size() - FRAME_HEADER_SIZE);
}

void G2FDB::FrameSequence::AddFrame(Frame &frame)
{
	frames_count++;
	end_time = frame.time;
	AddData(&frame.data[FRAME_HEADER_SIZE], frame.data.size() - FRAME_HEADER_SIZE);
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

bool G2FDB::G2fdbVolume::SetPointer(const LONGLONG &offset)
{
	return io.SetPointer(offset);
}

size_t G2FDB::G2fdbVolume::SignatureOffset(void)
{
	return 12;
}

bool G2FDB::G2fdbVolume::IsValidFrameHeader(const FRAME_HEADER &header)
{
	if (header.signature != FRAME_HEADER_SIGNATURE) {
		return false;
	}

	if (header.str_length > sizeof(FRAME_HEADER::camera_name)) {
		return false;
	}

	size_t len = strnlen_s((char *)header.camera_name, sizeof(header.camera_name));
	if (len != header.str_length) {
		return false;
	}

	if (header.payload_size == 0) {
		return false;
	}

	const size_t max_payload_size = 256 * 1024;

	if (header.payload_size > max_payload_size) {
		return false;
	}

	if ((header.width == 0) || (header.height == 0)) {
		return false;
	}

	return true;
}

bool G2FDB::G2fdbVolume::FindAndReadFrame(Frame &frame)
{
	LONGLONG offset = 0;
	FRAME_HEADER header = { 0 };
	uint32_t signature = FRAME_HEADER_SIGNATURE;

	while (io.Find((uint8_t *)&signature, sizeof(signature), offset)) {
		
		io.SetPointer(offset - SignatureOffset());

		if (ReadFrame(frame)) {
			return true;
		} else {
			io.SetPointer(offset + 1);
		}
	}

	return false;
}

bool G2FDB::G2fdbVolume::FindAndReadFrame(dvr::Frame &frame)
{
	LONGLONG offset = 0;
	FRAME_HEADER header = { 0 };
	uint32_t signature = FRAME_HEADER_SIGNATURE;

	while (io.Find((uint8_t *)&signature, sizeof(signature), offset)) {

		io.SetPointer(offset - SignatureOffset());

		if (ReadFrame(frame)) {
			return true;
		}
		else {
			io.SetPointer(offset + 1);
		}
	}

	return false;
}

bool G2FDB::G2fdbVolume::ReadFrame(G2FDB::Frame &frame)
{
	FRAME_HEADER *header = nullptr;
	size_t data_size = 0;
	LONGLONG offset = io.Pointer();

	frame.data.resize(FRAME_HEADER_SIZE);

	if (FRAME_HEADER_SIZE == io.Read(&frame.data[0], sizeof(FRAME_HEADER))) {
		header = (FRAME_HEADER *)&frame.data[0];
		if (IsValidFrameHeader(*header)) {	

			frame.offset = offset;
			frame.camera = header->camera_id + 1;
			frame.width = header->width;
			frame.height = header->height;
			header->time.Timestamp(frame.time);

			data_size = header->payload_size;
			frame.data.resize(FRAME_HEADER_SIZE + data_size);

			if (data_size == io.Read(&frame.data[FRAME_HEADER_SIZE], data_size)) {
				return true;
			}
		}
	}

	io.SetPointer(offset);
	frame.Clear();
	return false;
}

bool G2FDB::G2fdbVolume::ReadFrame(dvr::Frame &frame)
{
	FRAME_HEADER *header = nullptr;
	size_t data_size = 0;
	LONGLONG offset = io.Pointer();

	frame.buffer.resize(FRAME_HEADER_SIZE);

	if (FRAME_HEADER_SIZE == io.Read(&frame.buffer[0], sizeof(FRAME_HEADER))) {
		header = (FRAME_HEADER *)&frame.buffer[0];
		if (IsValidFrameHeader(*header)) {

			frame.offset = offset;
			frame.camera = header->camera_id + 1;
			frame.width = header->width;
			frame.height = header->height;
			header->time.Timestamp(frame.time);

			data_size = header->payload_size;
			frame.buffer.resize(FRAME_HEADER_SIZE + data_size);

			if (data_size == io.Read(&frame.buffer[FRAME_HEADER_SIZE], data_size)) {
				return true;
			}
		}
	}

	io.SetPointer(offset);
	frame.Clear();
	return false;
}

bool G2FDB::G2fdbVolume::ReadFrameSequence(G2FDB::FrameSequence &sequence, size_t max_delta_time)
{
	LONGLONG last_frame_offset = 0;
	Frame frame;

	if (FindAndReadFrame(frame)) {

		sequence.AddFirstFrame(frame);
		last_frame_offset = frame.offset;

		while(ReadFrame(frame)) {
			last_frame_offset = frame.offset;
			if ((frame.camera == sequence.camera) &&
				((sequence.data.size() + frame.data.size()) <= MAX_FRAME_SEQUANCE_SIZE) &&
				(frame.time.Seconds() >= sequence.end_time.Seconds()) && 
				((frame.time.Seconds() - sequence.end_time.Seconds()) <= max_delta_time)
				) {
				sequence.AddFrame(frame);
			} else {
				SetPointer(frame.offset);
				return true;
			}		
		}

		SetPointer(last_frame_offset + SignatureOffset() + 1);
		return true;	
	}

	sequence.Clear();
	return false;
}

bool G2FDB::G2fdbVolume::ReadFrameSequence(dvr::FrameSequence &sequence, size_t max_size, size_t max_delta_time)
{
	dvr::Frame frame;
	uint64_t last_frame_offset = 0;
	size_t last_frame_size = 0;
	size_t prev_buffer_size = 0;

	if (FindAndReadFrame(frame)) {
	
		sequence.AddFirstFrame(frame);
		last_frame_offset = frame.offset;
		last_frame_size = frame.buffer.size();

		while (ReadFrame(frame)) {
			last_frame_offset = frame.offset;
			last_frame_size = frame.buffer.size();
			if ((frame.camera == sequence.camera) &&
				((sequence.width == frame.width) && (sequence.height == frame.height)) &&
				((sequence.buffer.size() + frame.buffer.size()) <= max_size) &&
				(frame.time.Seconds() >= sequence.end_time.Seconds()) &&
				((frame.time.Seconds() - sequence.end_time.Seconds()) <= max_delta_time))
			{
				prev_buffer_size = sequence.buffer.size();
				sequence.AddFrame(frame);
			} else {
				SetPointer(frame.offset);
				return true;
			}		
		}

		SetPointer(last_frame_offset + SignatureOffset() + 1);

		if (FindAndReadFrame(frame)) {
			SetPointer(frame.offset);
			assert(frame.offset > last_frame_offset);
			if ((frame.offset - last_frame_offset) < last_frame_size) {
				sequence.buffer.resize(prev_buffer_size + (size_t)(frame.offset - last_frame_offset));
			}		
		}

		return true;
	}

	return false;
}
