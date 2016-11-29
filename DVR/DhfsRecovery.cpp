#include "DhfsRecovery.h"
#include "utility.h"
#include <cassert>


int DHFS::StartRecovery(const std::string & dvr_volume, const std::string & out_directory, const dvr::Timestamp & start_time, const dvr::Timestamp & end_time)
{
	DhfsVolume volume(dvr_volume);
	Storage storage(out_directory, 500*1024*1024LL);
	FrameSequence sequence;

	size_t max_delta_time = 2; // seconds
	size_t max_sequence_size = 100*1024*1024;

	if (volume.Open() && storage.Open()) {

		while (volume.FindAndReadFrameSequence(sequence, max_sequence_size)) {
			//storage.SaveFrameSequence(sequence);
			storage.SaveFrameSequenceEx(sequence, max_delta_time);
		}
		return 0;
	}

	return -1;
}

std::string DHFS::VFile::Description(void)
{
	std::string descr = string_format("[%02u]-[ %s -=- %s ]-[%ux%u]", camera,
		start_time.ToString().c_str(), end_time.ToString().c_str(), width, height);
	return descr;
}

bool DHFS::VFile::SaveFrameSequence(FrameSequence &sequence)
{
	if (io.Write(sequence.data.data(), sequence.data.size())) {
		if (start_time.Seconds() == 0) {
			camera = sequence.first_frame.camera;
			start_time = sequence.first_frame.time;
			width = sequence.width;
			height = sequence.height;
		}

		assert(width == sequence.width);
		assert(height == sequence.height);

		end_time = sequence.last_frame.time;
		last_frame_offset = sequence.last_frame.offset;
		return true;
	}
	return false;
}

DHFS::Storage::Storage(const std::string &storage_directory, LONGLONG max_file_size_) :
	base_directory(storage_directory),
	max_file_size(max_file_size_)
{	
}

DHFS::Storage::~Storage()
{
	Close();
}

bool DHFS::Storage::CreateDirectory(const std::string &directory)
{
	bool result = ::CreateDirectoryA(directory.c_str(), NULL);

	if (!result) {
		if (ERROR_ALREADY_EXISTS == ::GetLastError()) {
			result = true;
		}
	}

	return result;
}

bool DHFS::Storage::Open()
{
	while (base_directory.size()) {
		char ch = base_directory.back();
		if (isspace(ch)) {
			base_directory.pop_back();
		}
		else {
			if (ch != '\\') {
				base_directory += "\\";
			}
			break;
		}
	}

	bool result = ::CreateDirectoryA(base_directory.c_str(), NULL);
	if (!result) {
		if (ERROR_ALREADY_EXISTS == ::GetLastError()) {
			result = true;
		}
	}

	return result;
}

void DHFS::Storage::Close()
{
	for (auto substor_it : storage) {
		for (auto files_it : substor_it.second) {
			for (auto &file : files_it.second) {
				if (file != nullptr) {
					ConvertVideoFile(*file);
					::DeleteFileA(file->FileName().c_str());
					delete file;
					file = nullptr;
				}
			}		
		}	
	}
}

bool DHFS::Storage::SaveFrameSequence(FrameSequence & sequence)
{
	std::string date_dir = base_directory + sequence.first_frame.time.Date('-') + "\\";
	std::string cam_dir = date_dir + "CAM-" + std::to_string(sequence.first_frame.camera) + "\\";
	std::string temp_file_name = cam_dir + "dvr.tmp";

	if (!(sequence.width & sequence.height)) {
		return false;
	}

	if (CreateDirectory(date_dir) && CreateDirectory(cam_dir)) {

		W32Lib::FileEx temp_file(temp_file_name.c_str());
		if (temp_file.Create()) {

			if (temp_file.Write(&sequence.data[0], sequence.data.size())) {

				std::string out_file_name;

				out_file_name = string_format("%s[%02u]-[ %s -=- %s ]-[%ux%u].avi", cam_dir.c_str(), sequence.first_frame.camera,
					sequence.first_frame.time.ToString().c_str(), sequence.last_frame.time.ToString().c_str(),
					sequence.width, sequence.height);

				Convert2Avi(temp_file_name, out_file_name);

				return true;
			}
		}
	}

	return false;
}

void DHFS::Storage::ConvertVideoFile(VFile &file)
{
	file.Close();

	std::string file_name = file.FileName();
	std::string out_file_name = string_format("%s%s\\CAM-%02u\\%s.avi",
		base_directory.c_str(), file.StartTime().Date().c_str(), file.Camera(), file.Description().c_str());

	Convert2Avi(file_name, out_file_name);
}

bool DHFS::Storage::SaveFrameSequenceEx(FrameSequence &sequence, LONGLONG max_delta_time)
{
	LONGLONG max_distance = 10*1024*1024LL;

	std::string date = sequence.first_frame.time.Date();
	LONGLONG offset = sequence.first_frame.offset;

	VFile *vfile = nullptr;
	auto &substor = storage[date];
	auto &files = substor[sequence.first_frame.camera];
	for (auto vf : files) {
	
		if (vf->Width() == sequence.width && vf->Height() == sequence.height) {
			if (sequence.first_frame.time >= vf->EndTime()) {
				if ((sequence.first_frame.time - vf->EndTime()) <= max_delta_time) {
					vfile = vf;
					vfile->SaveFrameSequence(sequence);
					break;
				}			
			}	
		}
	}
	if (vfile == nullptr) {

		std::string date_dir = base_directory + sequence.first_frame.time.Date('-') + "\\";
		std::string cam_dir = string_format("%sCAM-%02u\\", date_dir.c_str(), sequence.first_frame.camera);
		std::string vfile_name = cam_dir + std::to_string(sequence.first_frame.offset) + ".h264";

		if (CreateDirectory(date_dir) && CreateDirectory(cam_dir)) {

			vfile = new VFile(vfile_name);
			if (vfile != nullptr) {
				if (vfile->Create()) {
					vfile->SaveFrameSequence(sequence);
					files.push_back(vfile);
				
				} else {
					delete vfile;
					vfile = nullptr;
				}
			}
		}	
	}	
	for (auto substor_it : storage) {
		for (auto files_it : substor_it.second) {
			std::list<VFile *> for_remove;
			for (auto file : files_it.second) {
				assert(file != nullptr);
				assert(sequence.last_frame.offset >= file->LastFrameOffset());
				if ((sequence.first_frame.offset - file->LastFrameOffset()) >= max_distance
					|| file->Size() >= max_file_size)
				{
					for_remove.push_back(file);
				} 
			}

			for (auto file : for_remove) {
				ConvertVideoFile(*file);
				::DeleteFileA(file->FileName().c_str());
				delete file;
				files_it.second.remove(file);
			}
		}
	}

	return false;
}