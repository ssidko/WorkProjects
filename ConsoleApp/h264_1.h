#pragma once
#include <windows.h>
#include <W32Lib.h>
#include <list>
#include <string>
#include <cstdlib>

namespace h264_1
{
#pragma pack (push)
#pragma pack(1)

	typedef enum {
		kUnknownType	= 0x00,
		kType_0A		= 0x0A,
		kType_0D		= 0x0D,
		kWithTimestamp	= 0x0C,
	} Type;

	typedef struct _TIMESTAMP {
		DWORD seconds:6;
		DWORD minutes:6;
		DWORD hours:5;
		DWORD day:5;
		DWORD month:4;
		DWORD year:6;	
	} TIMESTAMP;

#define HDR_MARK_1		((DWORD)0x010000)
#define HDR_MARK_2		((DWORD)0x0F)

	typedef struct _COMMON_HEADER {
		DWORD mark_1:24;	// 0x010000
		DWORD type:4;
		DWORD mark_2:4;		// 0x0F
		bool IsValid(void) { return ((this->mark_1 == HDR_MARK_1) && (this->mark_2 == HDR_MARK_2)); }
	} COMMON_HEADER;

	typedef struct  _SUBFRAME_TIME{
		COMMON_HEADER hdr;
		DWORD unknown_1;
		TIMESTAMP time;
		DWORD data_size;
	} SUBFRAME_TIME;

	typedef struct  _SUBFRAME_0A{
		COMMON_HEADER hdr;
		BYTE unk_1;
		BYTE unk_2;
		WORD data_size;
	} SUBFRAME_0A;

	typedef struct  _SUBFRAME_0D{
		COMMON_HEADER hdr;
		DWORD data_size;
	} SUBFRAME_0D;

	typedef struct  _SUBFRAME_HEADER{
		DWORD type;
		bool unknown_type;
		DWORD header_size;
		DWORD data_size;
		LONGLONG offset;
		TIMESTAMP timestamp;
	} SUBFRAME_HEADER;

#pragma pack (pop)


	typedef struct _FRAME_DESCRIPTOR{
		LONGLONG offset;
		TIMESTAMP timestamp;
		DWORD subframes;
		DWORD video_frames; 
		DWORD size;
		DWORD clean_size;

		bool IsComplete(void) {return (size == clean_size);}
		bool IsIncomplete(void) {return (size != clean_size);}
	} FRAME_DESCRIPTOR;

	class Timestamp
	{
	private:
		DWORD sec;
		DWORD mins;
		DWORD hours;
		DWORD day;
		DWORD mounth;
		DWORD year;
		LONGLONG seconds;
		char str[32];
	public:
		Timestamp() : sec(0),mins(0),hours(0),day(0),mounth(0),year(0),seconds(0) {}
		Timestamp(const TIMESTAMP &t)
		{
			sec = t.seconds;
			mins = t.minutes;
			hours = t.hours;
			day = t.day;
			mounth = t.month;
			year = 2000 + t.year;
			seconds = (LONGLONG)(((year*365 + mounth*30 + day)*24 + hours)*60 + mins)*60 + sec;
		}
		Timestamp(DWORD year_, DWORD month_, DWORD day_)
		{
			sec = 0;
			mins = 0;
			hours = 0;
			day = day_;
			mounth = month_;
			year = year_;
			seconds = (LONGLONG)(((year*365 + mounth*30 + day)*24 + hours)*60 + mins)*60 + sec;
		}
		LONGLONG Seconds(void)
		{
			return seconds;
		}
		const char *String(void)
		{
			memset(str, 0x00, sizeof(str));
			sprintf_s(str, sizeof(str), "%04d-%02d-%02d %02d-%02d-%02d",year,mounth,day,hours,mins,sec);
			return str;
		}
		bool operator>(const Timestamp &t)
		{
			return (this->seconds > t.seconds);
		}
		bool operator<(const Timestamp &t)
		{
			return (this->seconds < t.seconds);
		}
		bool operator==(const Timestamp &t)
		{
			return (this->seconds == t.seconds);
		}
		LONGLONG operator-(const Timestamp &t)
		{
			return (this->seconds - t.seconds);
		}
		void operator=(const TIMESTAMP &t)
		{
			sec = t.seconds;
			mins = t.minutes;
			hours = t.hours;
			day = t.day;
			mounth = t.month;
			year = 2000 + t.year;
			seconds = (LONGLONG)(((year*365 + mounth*30 + day)*24 + hours)*60 + mins)*60 + sec;
		}
	};

	class VideoFile
	{
	private:
		DWORD id;
		FileEx file;
		std::string file_path;
		Timestamp start_time;
		Timestamp last_time;
	public:
		VideoFile(const char *file_name, DWORD file_id) :  id(file_id), file_path(file_name) {}
		bool Create() {return (file.Create(file_path.data()) != 0x00);}
		const Timestamp *StartTime(void) {return &start_time;}
		const Timestamp *LastTime(void) {return &last_time;}
		bool SaveFrame(const FRAME_DESCRIPTOR &frame_descriptor, BYTE *frame_buff, DWORD frame_size)
		{
			if (start_time.Seconds() == 0x00) {
				start_time = frame_descriptor.timestamp;
			}
			last_time = frame_descriptor.timestamp;
			return (file.Write(frame_buff, frame_size) == frame_size);
		}
	};

#define MAX_DELTA_TIME				(LONGLONG)2
#define FRAME_BUFFER_SIZE			(DWORD)512*512

	class VideoStorage
	{
	private:
		BYTE *buffer;
		DWORD next_id;
		std::string dir_path;
		VideoFile *current_file;
		std::list<VideoFile *> files;
	public:
		VideoStorage(const char *directory) : buffer(NULL), next_id(0), dir_path(directory), current_file(NULL)
		{
			buffer = new BYTE[FRAME_BUFFER_SIZE];
		}
		~VideoStorage()
		{
			delete[] buffer;
		}
		DWORD NextID(void)
		{
			++next_id;
			if (next_id == 0x00) {
				++next_id;
			}
			return next_id;
		}

		VideoFile *CreateNewVideoFile(Timestamp &timestamp, DWORD id)
		{
			char id_str[16];
			memset(id_str, 0x00, sizeof(id_str));
			_itoa_s (id,id_str,sizeof(id_str),10);
			std::string file_name = dir_path + "\\" + timestamp.String() + " [" + id_str + "]";
			VideoFile *video_file = new VideoFile(file_name.c_str(), id);
			if (!video_file) {
				throw -1;
			}
			if (!video_file->Create()) {
				throw -1;
			}
			return video_file;
		}

		VideoFile *GetVideoFile(Timestamp &timestamp)
		{
			VideoFile *vf = NULL;
			LONGLONG delta = 0;
			if (files.size()){
				if (current_file) {
					if (timestamp > *current_file->LastTime()) {
						if ((timestamp - *current_file->LastTime()) <= MAX_DELTA_TIME) {
							return current_file;
						}
					}
				}
				for (std::list<VideoFile *>::iterator it = files.begin(); it != files.end(); ++it) {
					if (timestamp > *(*it)->LastTime()) {
						if ((timestamp - *(*it)->LastTime()) <= MAX_DELTA_TIME) {
							current_file = *it;
							return current_file;
						}
					}
				}
			} 

			current_file = CreateNewVideoFile(timestamp, NextID());
			files.push_back(current_file);
			return current_file;
		}

		bool SaveFrame(FileEx &file, const FRAME_DESCRIPTOR &frame)
		{
			Timestamp time = frame.timestamp;
			if (file.Read(buffer, frame.size) != frame.size) {
				return false;
			}
			VideoFile *video_file = GetVideoFile(time);
			if (video_file) {
				return video_file->SaveFrame(frame, buffer, frame.size);
			} else {
				return false;
			}
		}
	};

	bool ReadSubframeHeader(FileEx &file, LONGLONG &offset, SUBFRAME_HEADER &subframe_header);
	bool ReadSubframeHeader(FileEx &file, SUBFRAME_HEADER &subframe_header);

	int main(TCHAR *file_name, LONGLONG offset, TCHAR *out_dir);

}

