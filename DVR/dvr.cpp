#include "dvr.h"
#include "utility.h"
#include <stdio.h>
#include <cctype>
#include <assert.h>
#include "File.h"
#include <experimental\filesystem>

namespace fs = std::experimental::filesystem;


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

bool dvr::Timestamp::Valid(void) const
{
	if ((month >= 1) && (month <= 12)) {
		if ((day >=1) && (day <= 31)) {
			if (hours <= 24) {
				if (minutes <= 59) {
					if (seconds <= 59) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

ULONGLONG dvr::Timestamp::Seconds( void ) const
{
	return (ULONGLONG)((((year*12LL + month)*31LL + day)*24LL + hours)*60LL + minutes)*60LL + seconds;
}

std::string dvr::Timestamp::ToString(void) const
{
	std::string str;
	str.resize(32);
	str.resize(std::snprintf(&str[0], str.size(), "%04u-%02u-%02u - %02u-%02u-%02u", year, month, day, hours, minutes, seconds));
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
	width = 0;
	height = 0;
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

void dvr::FrameSequence::AddFirstFrame(Frame &frame)
{
	camera = frame.camera;
	first_frame_offset = frame.offset;
	start_time = frame.time;
	frames_count = 0;
	AddFrame(frame);
}

void dvr::FrameSequence::AddFrame(Frame &frame)
{
	assert(camera == frame.camera);

	if (!width || !height) {
		if (frame.width && frame.height) {
			width = frame.width;
			height = frame.height;
		}
	}

	last_frame_offset = frame.offset;
	frames_count++;
	end_time = frame.time;
	
	size_t origin_size = buffer.size();
	buffer.resize(origin_size + frame.buffer.size());
	std::memcpy(&buffer[origin_size], frame.buffer.data(), frame.buffer.size());
}


//
// Class VideoFile
//

dvr::VideoFile::VideoFile(const std::string & file_path, FrameSequence & seq) : io_path(file_path)
{
	first_frame_offset = seq.first_frame_offset;
	camera = seq.camera;
	start_time = seq.start_time;
	width = seq.width;
	height = seq.height;
	file_size = 0;

	W32Lib::FileEx io(this->io_path.c_str());
	if (!io.Create()) {
		throw std::system_error(::GetLastError(), std::system_category());
	}

	AppendFrameSequence(seq);
}

void dvr::VideoFile::AppendFrameSequence(FrameSequence & seq)
{
	last_frame_offset = seq.last_frame_offset;

	assert(camera == seq.camera);

	if (!width || !height) {
		if (seq.width && seq.height) {
			width = seq.width;
			height = seq.height;
		}
	}

	end_time = seq.end_time;

	W32Lib::FileEx io(this->io_path.c_str());
	if (io.Open()) {
		io.SetPointer(0, FILE_END);
		file_size += io.Write(seq.buffer.data(), seq.buffer.size());
	}
	else {
		throw std::system_error(::GetLastError(), std::system_category());
	}
}

//
// Class VideoStorage
//
dvr::VideoStorage::VideoStorage(const std::string &path) : dir_path(path)
{
}

dvr::VideoStorage::~VideoStorage(void)
{
	Close();
}

bool dvr::VideoStorage::Open(void)
{
	try {
		while (!dir_path.empty()) {
			if (std::isspace(dir_path.back())) {
				dir_path.pop_back();
				continue;
			} else if (dir_path.back() == '\\'){
				dir_path.pop_back();
				break;
			} 
			break;
		}

		CreateDir(dir_path);
		return true;
	} catch (...) {
		return false;
	}
}

void dvr::VideoStorage::Close(void)
{
	CloseAllFiles();
}


bool dvr::VideoStorage::SaveFrameSequence(FrameSequence &seq)
{
	VideoFile *file = GetVideoFile(seq.start_time.Date(), seq.camera);
	bool appended = false;

	if (file) {
		if ((file->Size() + seq.buffer.size()) <= max_file_size) {
			if (file->EndTimestamp().Seconds() <= seq.start_time.Seconds()) {
				if ((seq.start_time.Seconds() - file->EndTimestamp().Seconds()) <= 1) {
					file->AppendFrameSequence(seq);
					appended = true;
				}
			}
		}
	}

	if ((file == nullptr) || !appended) {
		file = CreateNewFile(seq);
	}

	assert(file != nullptr);

	//
	// Update recent files
	if (appended) {
		auto it = recent_files.begin();
		while (it != recent_files.end()) {			
			if (*it == file) {
				it = recent_files.erase(it);
				recent_files.push_back(file);
			} else {
				++it;
			}
		}		
	}	

	//
	// Close all far files
	while (!recent_files.empty()) {		
		if ((seq.last_frame_offset - recent_files.front()->LastFrameOffset()) >= max_distance) {
			CloseFile(recent_files.front());
		} else {
			break;
		}
	}

	//
	// Remove empty pairs
	auto it = storage.begin();
	while (it != storage.end()) {
		auto iit = (*it).second.begin();
		while (iit != (*it).second.end()) {
			if ((*iit).second == nullptr) {
				iit = (*it).second.erase(iit);
			} else {
				++iit;
			}
		}
		if ((*it).second.empty()) {
			it = storage.erase(it);
		} else {
			++it;
		}
	}

	return true;
}

dvr::VideoFile * dvr::VideoStorage::CreateNewFile(FrameSequence &seq)
{
	std::string file_name = std::to_string(seq.first_frame_offset) + " - [" + seq.start_time.ToString() + "]" + ".dvr";
	std::string date_dir = dir_path + std::string("\\") + seq.start_time.Date();
	std::string cam_dir = date_dir + std::string("\\") + std::string("CAM-") + std::to_string(seq.camera);
	std::string file_path = cam_dir + std::string("\\") + file_name;

	CreateDir(date_dir);
	CreateDir(cam_dir);

	VideoFile *video_file = GetVideoFile(seq.start_time.Date(), seq.camera);
	if (video_file) {
		CloseFile(video_file);
	}
	video_file = new VideoFile(file_path, seq);

	storage[seq.start_time.Date()][seq.camera] = video_file;
	recent_files.push_back(video_file);

	return video_file;
}

void dvr::VideoStorage::CloseFile(dvr::VideoFile *file)
{
	assert(file != nullptr);

	std::string file_name = format_string("[%02d]-[%s -=- %s]-[%dx%d]-%lld-%lld",
		file->Camera(),
		file->StartTimestamp().ToString().c_str(), file->EndTimestamp().ToString().c_str(),
		file->Width(), file->Height(), file->FirstFrameOffset(), file->LastFrameOffset());

	file->Close();

	fs::path current_file_path = file->FilePath();
	fs::path dir_path = current_file_path.parent_path();

	fs::path native_file_path = dir_path / (file_name + ".h264");
	fs::path avi_file_path = dir_path / (file_name + ".avi");

	if (!fs::exists(native_file_path)) {

		fs::rename(current_file_path, native_file_path);
		Convert2Avi(native_file_path.string(), avi_file_path.string());
		fs::remove(native_file_path);
		if (fs::exists(avi_file_path) && (fs::file_size(avi_file_path) == 0)) {
			fs::remove(avi_file_path);
		}

	} else {		
		fs::remove(current_file_path);
	}


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
	while (!recent_files.empty()) {
		CloseFile(recent_files.front());
	}
}

dvr::VideoFile * dvr::VideoStorage::GetVideoFile(const std::string &date, uint32_t camera)
{
	auto it = storage.find(date);
	if (it != storage.end()) {
		auto itt = it->second.find(camera);
		if (itt != it->second.end()) {
			if (itt->second) {
				assert(itt->second->Camera() == camera);
				assert(itt->second->StartTimestamp().Date() == date);
			}
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
