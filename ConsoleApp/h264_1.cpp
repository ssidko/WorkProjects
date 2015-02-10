#include "h264_1.h"



namespace h264_1
{
	bool ReadSubframeHeader(FileEx &file, LONGLONG &offset, SUBFRAME_HEADER &subframe_header)
	{
		file.SetPointer(offset);
		return ReadSubframeHeader(file, subframe_header);
	}

	bool ReadSubframeHeader(FileEx &file, SUBFRAME_HEADER &subframe_header)
	{
		BYTE buff[sizeof(SUBFRAME_TIME)] = {0};
		COMMON_HEADER *header = (COMMON_HEADER *)buff;
		SUBFRAME_TIME *subframe_time = (SUBFRAME_TIME *)buff;
		SUBFRAME_0A *subframe_0a = (SUBFRAME_0A *)buff;
		SUBFRAME_0D *subframe_0d = (SUBFRAME_0D *)buff;

		subframe_header.Clear();
		file.GetPointer(&subframe_header.offset);

		if (file.Read(header, sizeof(buff)) == sizeof(buff)) {
			file.SetPointer(subframe_header.offset);
			if (header->IsValid()) {
				subframe_header.unknown_type = false;
				subframe_header.type = header->type;
				switch (header->type) {
					case kWithTimestamp :
						subframe_header.timestamp = subframe_time->time;
						subframe_header.header_size = sizeof(SUBFRAME_TIME);
						subframe_header.data_size = subframe_time->data_size;
						break;

					case kType_0A :
						subframe_header.header_size = sizeof(SUBFRAME_0A);
						subframe_header.data_size = subframe_0a->data_size;
						break;

					case kType_0D :
						subframe_header.header_size = sizeof(SUBFRAME_0D);
						subframe_header.data_size = subframe_0d->data_size;
						break;

					default:
						subframe_header.unknown_type = true;
						break;
				}
				return true;
			}
		}
		return false;
	}

	bool NextFrame(FileEx &file, LONGLONG &start_offset, FRAME_DESCRIPTOR &frame)
	{
		BYTE magic[] = {0x00, 0x00, 0x01, 0xFC};
		
		file.SetPointer(start_offset);
		while (file.Find(magic, sizeof(magic)) != (-1)) {

			SUBFRAME_HEADER hdr;
			DWORD prev_size = 0;
			bool is_present_timestamp_subframe = false;

			frame.Clear();
			while (ReadSubframeHeader(file, hdr)) {

				if (!hdr.unknown_type) {
					if (hdr.type == kWithTimestamp) {
						if (!is_present_timestamp_subframe) {
							is_present_timestamp_subframe = true;
							frame.timestamp = hdr.timestamp;
							frame.offset = hdr.offset;
						} else {
							frame.clean_size = frame.size;
							break;
						}
					}

					frame.subframes++;
					if (hdr.IsVideoFrame()) {
						frame.video_frames++;
					}
					frame.size += (hdr.header_size + hdr.data_size);
					frame.clean_size += prev_size;
					file.SetPointer((LONGLONG)(hdr.header_size + hdr.data_size), FILE_CURRENT);

					prev_size = (hdr.header_size + hdr.data_size);

				} else {
					break;
				}
			}
			if (frame.offset) {
				file.SetPointer(frame.offset);
			} else {
				file.SetPointer(start_offset);
			}
			if (frame.size) {
				return true;
			}
		}
		return false;
	}

	bool NextFrameSequence(FileEx &file, LONGLONG &start_offset, FRAME_SEQUENCE &sequence)
	{
		LONGLONG offset = start_offset;
		FRAME_DESCRIPTOR frame = {0};
		sequence.Clear();
		while (NextFrame(file, offset, frame)) {

			if (sequence.frame_count == 0x00) {
				sequence.start_time = frame.timestamp;
				sequence.offset = frame.offset;
			}
			sequence.end_time = frame.timestamp;
			sequence.frame_count++;

			if (frame.IsComplete()) {
				offset = (frame.offset + frame.clean_size);
				sequence.next_offset = offset;
				sequence.size += frame.clean_size;

			} else {
				offset = (frame.offset + frame.clean_size + 1);
				sequence.next_offset = offset;
				break;
			}
		}
		return (sequence.frame_count != 0x00);
	}

#define BUFFER_SIZE				DWORD(512*512)

	int main(TCHAR *file_name, LONGLONG offset, TCHAR *out_dir)
	{
		FileEx file(_T("G:\\36829\\original.dsk"));
		FileEx log_file(_T("G:\\36829\\headers-timestamp.log"));
		char log_str[128];
		
		VideoStorage storage("G:\\36829\\out");
		Timestamp min_time(2014, 6, 1);
		Timestamp max_time(2014, 12, 1);
		Timestamp time;
		if (file.Open() && log_file.Create()) {
			LONGLONG offset = 0x62F369C2B2;
			LONGLONG max_size = 0;
			FRAME_DESCRIPTOR frame = {0};
			while (NextFrame(file, offset, frame)) {

				time = frame.timestamp;
				memset(log_str, 0x00, sizeof(log_str));
				sprintf_s(log_str, sizeof(log_str), "%08llX %s %02d/%02d %d/%d\n",
					frame.offset, time.String(),
					frame.video_frames,
					frame.subframes,
					frame.clean_size,
					frame.size);
				log_file.Write(log_str, strlen(log_str));

				if (frame.IsComplete()) {
					offset = (frame.offset + frame.clean_size);
				} else {
					offset = (frame.offset + frame.clean_size + 1);
					memset(log_str, 0x00, sizeof(log_str));
					sprintf_s(log_str, sizeof(log_str), "\n# New frame sequence --->\n");
					log_file.Write(log_str, strlen(log_str));
				}

				if ( (time > min_time) && (time < max_time) ) {
					storage.SaveFrame(file, frame);
				}
				int x = 0;
			}
		}
		return 0;
	}

}