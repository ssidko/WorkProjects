#include <assert.h>
#include "Frame.h"
#include <sstream>

using namespace DHFS;


Frame::Frame(FRAME_HEADER &frame_header, LONGLONG &frame_offset) :
	buffer(NULL),
	offset(frame_offset),
	footer(NULL)
{
	header = new FRAME_HEADER();
	*header = frame_header;
	timestamp = Timestamp((header->time.year + 2000), header->time.month, header->time.day, header->time.hours, header->time.minutes, header->time.seconds);
}

Frame::Frame(BYTE *frame_buffer, LONGLONG &frame_offset) :
	buffer(frame_buffer),
	offset(frame_offset),
	header((FRAME_HEADER *)frame_buffer),
	footer(NULL)
{
	assert(frame_buffer);
	timestamp = Timestamp((header->time.year + 2000), header->time.month, header->time.day, header->time.hours, header->time.minutes, header->time.seconds);
}

Frame::~Frame(void)
{
	if (buffer) {
		delete[] buffer;
	} else if (header) {
		delete[] header;
	}
}

bool DHFS::Frame::IsValidHeader(FRAME_HEADER *hdr)
{
	return (hdr->size <= FRAME_MAX_SIZE);
}

Frame *Frame::NextFrame(BufferedFile &file)
{
	LONGLONG offset = 0;
	FRAME_HEADER header = {0};
	FRAME_FOOTER footer = {0};
	DWORD header_magic = FRAME_HEADER_MAGIC;

	while ( offset = file.Find((BYTE *)&header_magic, sizeof(header_magic)) ) {
		if (file.Read((void *)&header, sizeof(header)) != sizeof(header)) {
			return NULL;
		}
		if (IsValidHeader(&header)) {
			file.SetPointer(offset + header.size - sizeof(footer));
			if (file.Read((void *)&footer, sizeof(footer)) != sizeof(footer)) {
				return false;
			}
			if ((footer.magic == FRAME_FOOTER_MAGIC) && (header.size == footer.size)) {
				/*We have valid Frame*/
				Frame *frame = new Frame(header, offset);
				assert(frame);
				file.SetPointer(offset + header.size);
				return frame;
			}
		}
		file.SetPointer(offset + 1);
	}
	return NULL;
}

bool DHFS::Frame::IsNextFrame( Frame &next_frame )
{
	if (next_frame.Header().camera == header->camera) {
		if (next_frame.Time().Seconds() >= timestamp.Seconds()) {
			if ((next_frame.Time().Seconds() - timestamp.Seconds()) <= 2) {
				if ((next_frame.Header().counter - header->counter) <= 2) {
					return true;
				}
			}
		}
	}
	return false;
}

bool DHFS::Frame::NextFrameSequence(BufferedFile &file, FrameSequence &sequence)
{
	Frame *frame = NULL;
	Frame *prev_frame = NULL;

	sequence.Clear();
	while (frame = Frame::NextFrame(file)) {
		if (prev_frame) {
			if (prev_frame->IsNextFrame(*frame)) {
				sequence.end_time = frame->Time();
				sequence.frame_count++;
				sequence.end_counter = frame->Header().counter;
				sequence.size += frame->Header().size;
			} else {

				file.SetPointer(frame->Offset());
				delete frame; frame = NULL;
				delete prev_frame; prev_frame = NULL;
				
				if (sequence.frame_count >= 2) {
					return true;
				} else {
					sequence.Clear();
					continue;
				}
			}
			delete prev_frame; prev_frame = NULL;

		} else {
			sequence.start_time = frame->Time();
			sequence.end_time = frame->Time();
			sequence.camera = frame->Header().camera;
			sequence.offset = frame->Offset();
			sequence.frame_count++;
			sequence.start_counter = frame->Header().counter;
			sequence.size += frame->Header().size;
		}
		prev_frame = frame;
	}
	if (frame) {
		delete frame;
	}
	if (prev_frame) {
		delete prev_frame;
	}
	return false;
}

std::string DHFS::Frame::Info( void )
{
	std::stringstream stream;
	stream << offset << " ";
	stream << timestamp.String() << " ";
	stream << "cam: " << header->camera << " ";
	stream << "cnt: " << header->counter << " ";
	stream << "flags: " << header->flags << " ";
	stream << "\n";
	return stream.str();
}

void DHFS::_FrameSequence::Info( std::string &info_str )
{
	std::stringstream stream;

	stream << offset << " - ";
	stream << "cam: " << camera << ", ";
	stream << start_time.String() << "-" << end_time.String() << ", ";
	stream << "frames: " << frame_count << ", ";
	stream << "size: " << size ;
	stream << ".\n";

	info_str = stream.str();	
}
