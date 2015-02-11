#pragma once
#include <windows.h>
#include <W32Lib.h>
#include <list>
#include <string>
#include <cstdlib>
#include <sstream>
#include <iostream>

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

		void Clear(void)
		{
			memset(this, 0x00, sizeof(*this));
		}
		bool IsVideoFrame(void) {return (type == kType_0D) || (type == kWithTimestamp);}
	} SUBFRAME_HEADER;

#pragma pack (pop)


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
		void Clear(void)
		{
			memset(this, 0x00, sizeof(*this));
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

	typedef struct _FRAME_DESCRIPTOR{
		LONGLONG offset;
		TIMESTAMP timestamp;
		DWORD subframes;
		DWORD video_frames; 
		DWORD size;
		DWORD clean_size;

		void Clear(void)
		{
			memset(this, 0x00, sizeof(*this));
		}
		bool IsComplete(void) {return (size == clean_size);}
		bool IsIncomplete(void) {return (size != clean_size);}
		std::string Info(void)
		{
			char str[128] = {0};
			Timestamp time = timestamp;
			sprintf_s(str, sizeof(str), "%012llX %s %03d/%03d %06d/%06d\n",
				offset, time.String(),
				video_frames,
				subframes,
				clean_size,
				size);
			return std::string(str);
		}
	} FRAME_DESCRIPTOR;

	typedef struct _FRAME_SEQUENCE {
		Timestamp start_time;
		Timestamp end_time;
		LONGLONG offset;
		LONGLONG next_offset;
		DWORD size;
		DWORD frame_count;

		void Clear(void)
		{
			start_time.Clear();
			end_time.Clear();
			offset = 0;
			size = 0;
			frame_count = 0;
		}
	} FRAME_SEQUENCE;

#define MAX_DELTA_TIME				(LONGLONG)2
#define MAX_FRAME_SIZE				(DWORD)512*1024

	class VideoFile
	{
	private:
		LONGLONG id;
		FileEx file;
		std::string file_path;
		Timestamp start_time;
		Timestamp last_time;
	public:
		VideoFile(const char *file_name, LONGLONG file_id) :  id(file_id), file_path(file_name) {}
		bool Create(void) {return (file.Create(file_path.data()) != 0x00);}
		void Close(void) {file.Close();}
		Timestamp *StartTime(void) {return &start_time;}
		Timestamp *LastTime(void) {return &last_time;}
		std::string Name(void) {return file_path;}
		bool SaveFrame(const FRAME_DESCRIPTOR &frame_descriptor, BYTE *frame_buff, DWORD frame_size)
		{
			if (start_time.Seconds() == 0x00) {
				start_time = frame_descriptor.timestamp;
			}
			last_time = frame_descriptor.timestamp;
			return (file.Write(frame_buff, frame_size) == frame_size);
		}
		bool SaveFrameSequence(FRAME_SEQUENCE &sequence, BYTE *buff, DWORD size)
		{
			if (start_time.Seconds() == 0x00) {
				start_time = sequence.start_time;
			}
			last_time = sequence.end_time;
			return (file.Write(buff, sequence.size) == sequence.size);
		}
		bool ItsYour(Timestamp &time)
		{
			if (time.Seconds() >=  last_time.Seconds()) {
				if ((time.Seconds() - last_time.Seconds()) <= MAX_DELTA_TIME) {
					return true;
				}
			}
			
			return false;
		}
		LONGLONG Size(void)
		{
			return file.GetSize();
		}
		LONGLONG Id(void) {return id;}
	};

	class VideoStorage
	{
	private:
		DWORD next_id;
		std::string dvr_dir;
		std::string mkv_dir;
		VideoFile *current_file;
		std::list<VideoFile *> files;
	public:
		VideoStorage(const char *dvr_directory, const char *mkv_directory) : next_id(0), dvr_dir(dvr_directory), mkv_dir(mkv_directory), current_file(NULL)	{}
		~VideoStorage() {}

		LONGLONG NextID(void)
		{
			++next_id; 
			if (next_id == 0x00) {
				++next_id;
			}
			return next_id;
		}

		VideoFile *CreateNewFile(Timestamp &timestamp, LONGLONG id)
		{
			std::stringstream file_name;
			file_name << dvr_dir << "\\" << timestamp.String() << " " << id << ".dvr";
			VideoFile *video_file = new VideoFile(file_name.str().c_str(), id);
			if (!video_file || !video_file->Create()) {
				throw -1;
			}
			return video_file;
		}

		void CloseAndRemoveFile(VideoFile *file)
		{
			file->Close();
			files.remove(file);
			delete file;
		}

		VideoFile *GetFileFor(FRAME_SEQUENCE &sequence)
		{
			bool found = false;
			VideoFile *video_file = NULL;
			std::list<VideoFile *>::iterator it;
			std::list<VideoFile *> tmp_list;
			for (it = files.begin(); it != files.end(); ++it) {
				if ((*it)->ItsYour(sequence.start_time)) {
					tmp_list.push_back(*it);
				}
			}
			size_t count = tmp_list.size();
			if (count > 1) {
				for (it = tmp_list.begin(); it != tmp_list.end(); ++it) {
					CloseAndRemoveFile(*it);
				}
			} else if (count == 1) {
				video_file = *tmp_list.begin();
			}
			if (video_file == NULL) {
				video_file = CreateNewFile(sequence.start_time, NextID());
				files.push_back(video_file);
			}
			return video_file;
		}

		void ConvertToMkv(VideoFile &file)
		{
			std::string t;
			//std::string convertor_app("C:\\Program Files\\MKVToolNix\\mkvmerge.exe ");
			std::string convertor_app("C:\\MKVToolNix\\mkvmerge.exe ");

			std::stringstream src_file_path;
			src_file_path << "\"" << file.Name() << "\"";
			t = src_file_path.str();

			std::stringstream src_file_name;
			src_file_name << file.StartTime()->String() << " " << file.Id() << ".dvr";
			t = src_file_name.str();

			std::stringstream out_file_path;
			out_file_path << "\"" <<  mkv_dir << "\\" << src_file_name.str() << ".mkv" << "\"";
			t = out_file_path.str();

			std::stringstream cmd_line;
			cmd_line << convertor_app << "-o " << out_file_path.str() << " " << src_file_path.str();
			t = cmd_line.str();
			const char *zzz = t.data();
			system(cmd_line.str().data());
		}

		bool SaveFrameSequence(FileEx &file,  FRAME_SEQUENCE &sequence)
		{
			bool result = false;
			LONGLONG max_file_size = (LONGLONG)512*1024*1024;
			static VideoFile *video_file = NULL;

			DWORD rw = 0;
			BYTE *buffer = new BYTE[sequence.size];
			file.SetPointer(sequence.offset);
			if (file.Read(buffer, sequence.size) == sequence.size) {
				if(video_file && (video_file->Size() >= max_file_size)) {
					video_file->Close();
					ConvertToMkv(*video_file);
					delete video_file;
					video_file = NULL;
				}
				if (video_file == NULL) {
					video_file = CreateNewFile(sequence.start_time, sequence.offset);
				}

				result = video_file->SaveFrameSequence(sequence, buffer, sequence.size);
			}
			delete[] buffer;

			return result;
		}
	};

	bool ReadSubframeHeader(FileEx &file, LONGLONG &offset, SUBFRAME_HEADER &subframe_header);
	bool ReadSubframeHeader(FileEx &file, SUBFRAME_HEADER &subframe_header);

	int main(TCHAR *file_name, LONGLONG offset, TCHAR *out_dir);

}

