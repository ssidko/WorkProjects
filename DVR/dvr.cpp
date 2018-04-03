#include "dvr.h"
#include "utility.h"
#include <stdio.h>
#include <cctype>
#include <assert.h>

dvr::Timestamp::Timestamp(WORD year_, BYTE month_, BYTE day_, BYTE hours_, BYTE mins_, BYTE sec_) :
	year(year_),
	month(month_),
	day(day_),
	hours(hours_),
	minutes(mins_),
	seconds(sec_)
{
}

void dvr::Timestamp::Clear( void )
{
	year = month = day = hours = minutes = seconds = 0;
}

ULONGLONG dvr::Timestamp::Seconds( void ) const
{
	return (ULONGLONG)((((year*12LL + month)*31LL + day)*24LL + hours)*60LL + minutes)*60LL + seconds;
}

std::string dvr::Timestamp::ToString(void)
{
	char str[32];
	memset(str, 0x00, sizeof(str));
	sprintf_s(str, sizeof(str), "%04u-%02u-%02u--%02u-%02u-%02u", year, month, day, hours, minutes, seconds);
	return std::string(str);
}

void dvr::Timestamp::ToString(std::string & str)
{
	str = ToString();
}

std::string dvr::Timestamp::Date(char separator)
{
	char str[12];
	memset(str, 0x00, sizeof(str));
	separator = std::isprint(separator) ? separator : ' ';
	sprintf_s(str, sizeof(str), "%04u%c%02u%c%02u", year, separator, month, separator, day);
	return std::string(str);
}

std::string dvr::Timestamp::Time(char separator)
{
	char str[12];
	memset(str, 0x00, sizeof(str));
	separator = std::isprint(separator) ? separator : ' ';
	sprintf_s(str, sizeof(str), "%02u%c%02u%c%02u", hours, separator, minutes, separator, seconds);
	return std::string(str);
}

bool dvr::Timestamp::operator>( const Timestamp &t )
{
	return (this->Seconds() > t.Seconds());
}

bool dvr::Timestamp::operator>=( const Timestamp &t )
{
	return (this->Seconds() >= t.Seconds());
}

bool dvr::Timestamp::operator<( const Timestamp &t )
{
	return (this->Seconds() < t.Seconds());
}

bool dvr::Timestamp::operator<=( const Timestamp &t )
{
	return (this->Seconds() <= t.Seconds());
}

bool dvr::Timestamp::operator==( const Timestamp &t )
{
	return (this->Seconds() == t.Seconds());
}

ULONGLONG dvr::Timestamp::operator-( const Timestamp &t )
{
	return (this->Seconds() - t.Seconds());
}

dvr::Timestamp::operator bool(void)
{
	return year ? true : false;
}

void dvr::Frame::Clear(void)
{
	offset = 0;
	camera = 0;
	time.Clear();
	buffer.clear();
}

void dvr::FrameSequence::Clear(void)
{
	first_frame_offset = 0;
	last_frame_offset = 0;
	camera = 0;
	frames_count = 0;
	start_time.Clear();
	end_time.Clear();
	buffer.clear();
}

bool dvr::VideoStorage::SaveFrameSequence(FrameSequence &seq)
{
	VideoFile *file = GetVideoFile(seq.start_time.Date(), seq.camera);
	if (file && file->Size() > max_file_size) {
		CloseFile(file);
	}
	if (file == nullptr) {
		file = CreateNewFile(seq);
	}
	file->AppendFrameSequence(seq);

	return false;
}

dvr::VideoFile * dvr::VideoStorage::CreateNewFile(FrameSequence &seq)
{
	std::string file_name = std::to_string(seq.first_frame_offset) + seq.start_time.ToString() +  ".dvr";
	std::string date_dir = dir_path + std::string("\\") + seq.start_time.Date();
	std::string cam_dir = date_dir + std::string("\\") + std::string("CAM-") + std::to_string(seq.camera);
	std::string file_path = cam_dir + file_name;

	CreateDir(date_dir);
	CreateDir(cam_dir);

	VideoFile * video_file = new VideoFile(file_path);

	storage[seq.start_time.Date()][seq.camera] = video_file;


	return nullptr;	
}

dvr::VideoFile * dvr::VideoStorage::GetVideoFile(const std::string &date, uint32_t camera)
{
	auto it = storage.find(date);
	if (it != storage.end()) {
		auto itt = it->second.find(camera);
		if (itt != it->second.end()) {
			assert(itt->second);
			return itt->second;
		}
	} 
	return nullptr;
}

