#include "dvr.h"
#include "utility.h"
#include <stdio.h>
#include <cctype>
#include <assert.h>
//#include <filesystem>
//
//namespace fs = std::experimental::filesystem;

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

std::string dvr::Timestamp::ToString(void) const
{
	std::string str;
	str.resize(32);
	str.resize(std::snprintf(&str[0], str.size(), "%04u-%02u-%02u--%02u-%02u-%02u", year, month, day, hours, minutes, seconds));
	return str;
}	

void dvr::Timestamp::ToString(std::string & str) const 
{
	str = std::move(ToString());
}

std::string dvr::Timestamp::Date(char separator) const 
{
	std::string str = "";
	str.resize(12);
	separator = std::isprint(separator) ? separator : ' ';
	str.resize(std::snprintf(&str[0], str.size(), "%04u%c%02u%c%02u", year, separator, month, separator, day));
	return str;
}

std::string dvr::Timestamp::Time(char separator) const 
{
	std::string str = "";
	str.resize(12);
	separator = std::isprint(separator) ? separator : ' ';
	str.resize(std::snprintf(&str[0], str.size(), "%02u%c%02u%c%02u", hours, separator, minutes, separator, seconds));
	return str;
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
	height = 0;
	width = 0;
	frames_count = 0;
	start_time.Clear();
	end_time.Clear();
	buffer.clear();
}

bool dvr::VideoStorage::SaveFrameSequence(FrameSequence &seq)
{
	VideoFile *file = GetVideoFile(seq.start_time.Date(), seq.camera);
	if (file && ((file->Size() + seq.buffer.size()) > max_file_size)) {
		CloseFile(file);
		file = nullptr;
	}
	if (file == nullptr) {
		file = CreateNewFile(seq);
	} else {

		file->AppendFrameSequence(seq);

		//
		// UpdateRecentFiles(file)
		// CloseOldFiles();
		//


	}	

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
	recent_files.push_back(video_file);

	return video_file;
}

void dvr::VideoStorage::CloseFile(dvr::VideoFile *file)
{
	assert(file != nullptr);
	//
	// [01]-[2018-01-23 14-25-10 -=- 2018-01-23 14-27-30]-[800x600].avi
	//
	char file_name_format[] = "[%02d]-[%s -=- %s]-[%dx%d].avi";
	std::string file_name = "";
	file_name.resize(100);

	file_name.resize(std::snprintf(&file_name[0], file_name.size(), file_name_format,
		file->Camera(),
		file->StartTimestamp().ToString().c_str(), file->EndTimestamp().ToString().c_str(),
		file->Width(), file->Height()
		));

	file->Close();

	std::string dir_path(file->FilePath());
	while(!dir_path.empty()) {
		if (dir_path.back() != '\\') {
			dir_path.pop_back();
		} else {
			break;
		}
	}

	std::string avi_file_path = dir_path + file_name;
	Convert2Avi(file->FilePath(), avi_file_path);

	//
	// Remove from storage
	//
	assert (this->storage[file->StartTimestamp().Date()][file->Camera()] == file);
	this->storage[file->StartTimestamp().Date()][file->Camera()] = nullptr;


	//
	// Remove from recent_files
	//
	auto it = recent_files.begin();
	while(it != recent_files.end()) {
		if (*it == file) {
			recent_files.erase(it);
			break;
		} else {
			++it;
		}
	}

	delete file;
}
		
void dvr::VideoStorage::CloseAllFiles()
{
	while (recent_files.size()) {
		CloseFile(recent_files.front());
	}
}

dvr::VideoFile * dvr::VideoStorage::GetVideoFile(const std::string &date, uint32_t camera)
{
	auto it = storage.find(date);
	if (it != storage.end()) {
		auto itt = it->second.find(camera);
		if (itt != it->second.end()) {
			return itt->second;
		}
	} 
	return nullptr;
}



//
// ====================================================================
// ====================================================================
//

struct VFile {
	std::string file_name = "";
	dvr::Timestamp start_tsmpt;
	dvr::Timestamp end_tsmpt;
	uint64_t last_frame_offset = 0;
};

using CameraId = size_t;
using DateStr = std::string;

class CameraStorage
{
	CameraId camera_id = 0;
	size_t width = 0;
	size_t height = 0;
	std::string dir_path;
	std::map<CameraId, VFile *> storage;
public:
	CameraStorage() {}
	void SaveFrameSequence(const dvr::FrameSequence &seq)
	{
		
	}

};

class DateStorage
{
	DateStr date;
	std::string dir_path;
	std::map<CameraId, CameraStorage> storage;

public:
	DateStorage(DateStr date_, std::string directory_path) : date(date_), dir_path(directory_path)
	{
		CreateDir(dir_path);
	}

	CameraStorage & GetCameraStorage(CameraId camera)
	{
		
	}

};

class Storage
{
	std::string dir_path;
	std::map<DateStr, DateStorage> storage;
public:
	bool SaveFrameSequence(const dvr::FrameSequence &seq)
	{
		DateStr date = seq.start_time.Date();
		CameraId camera_id = seq.camera;

		//auto it = storage.find(date);
		//if (it == storage.end()) {
		//	storage.emplace(date, DateStorage(date, dir_path + "\\" + date));
		//	
		//}

		//DateStorage &data_stor = storage[date];
		//storage[date].GetCameraStorage(camera_id).SaveFrameSequence(seq);

		// RemoveOldFiles();

	}

	void Close() {}

};

dvr::VideoFile::VideoFile(const std::string & file_path, FrameSequence & seq) : io_path(file_path)
{
	camera = seq.camera;
	start_time = seq.start_time;
	AppendFrameSequence(seq);
}

bool dvr::VideoFile::AppendFrameSequence(FrameSequence & seq)
{
	last_frame_offset = seq.last_frame_offset;
	
	assert(camera == seq.camera);
	
	width = 0;
	height = 0;

	if (seq.width && seq.height) {
		if (width && height) {

		}	
	}
	



	end_time = seq.end_time;

	file_size += seq.buffer.size();

	return false;
}
