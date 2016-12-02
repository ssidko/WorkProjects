#include "DhfsRecovery.h"
#include "utility.h"
#include <cassert>


int DHFS::StartRecovery(const std::string & dvr_volume, const std::string & out_directory, const dvr::Timestamp & start_time, const dvr::Timestamp & end_time)
{
	LONGLONG file_size = FileSize(dvr_volume);
	if (!file_size) {
		return -1;
	}

	DhfsVolume volume(dvr_volume, file_size);
	Storage storage(out_directory, 500 * 1024 * 1024LL);
	FrameSequence sequence;

	size_t max_delta_time = 1 * 60; // seconds
	size_t max_sequence_size = 100 * 1024 * 1024;

	if (volume.Open() && storage.Open()) {

		while (volume.FindAndReadFrameSequence(sequence, max_sequence_size)) {
			//storage.SaveFrameSequence(sequence);
			if (sequence.width && sequence.height) {
				storage.SaveFrameSequenceEx(sequence, max_delta_time);
			}
		}
		return 0;
	}

	return -1;
}

bool DHFS::VFile::Create(void)
{
	if (io.Create()) {
		io.Close();
		return true;
	}
	return false;
}

void DHFS::VFile::Close(void)
{
	io.Close();
}

std::string DHFS::VFile::Description(void)
{
	std::string descr = string_format("[%02u]-[ %s -=- %s ]-[%ux%u]", camera,
		start_time.ToString().c_str(), end_time.ToString().c_str(), width, height);
	return descr;
}

LONGLONG DHFS::VFile::Size(void)
{
	LONGLONG size = 0;
	if (io.Open()) {
		size = io.GetSize();
		io.Close();
	}
	return size;
}

bool DHFS::VFile::SaveFrameSequence(FrameSequence &sequence)
{
	bool result = false;

	if (io.Open()) {
		io.SetPointer(io.GetSize());
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

			result = true;
		}	
		io.Close();
	}
	return result;
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
				assert(file != nullptr);
				CloseVideoFile(*file);
			}		
		}	
	}
}

void DHFS::Storage::CloseVideoFile(VFile &file)
{
	ConvertVideoFile(file);
	
	DWORD error = 0;
	if (!::DeleteFileA(file.FileName().c_str())) {
		error = ::GetLastError();
	}
	delete &file;
}

void DHFS::Storage::ConvertVideoFile(VFile &file)
{
	file.Close();

	std::string file_name = file.FileName();
	std::string out_file_name = string_format("%s%s\\CAM-%02u\\%s.avi",
		base_directory.c_str(), file.StartTime().Date().c_str(), file.Camera(), file.Description().c_str());

	//MoveFileA(file_name.c_str(), out_file_name.c_str());

	Convert2Avi(file_name, out_file_name);
}

bool DHFS::Storage::SaveFrameSequence(FrameSequence & sequence)
{
	std::string date_dir = base_directory + sequence.first_frame.time.Date('-') + "\\";
	std::string cam_dir = date_dir + "CAM-" + std::to_string(sequence.first_frame.camera) + "\\";
	std::string temp_file_name = cam_dir + "dvr.tmp";

	static LONGLONG skiped_size = 0;

	if (!(sequence.width & sequence.height)) {
		skiped_size += sequence.data.size();
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

bool DHFS::Storage::SaveFrameSequenceEx(FrameSequence &sequence, LONGLONG max_delta_time)
{
	LONGLONG max_distance = 1*1024*1024*1024LL;

	std::string date = sequence.first_frame.time.Date();
	LONGLONG offset = sequence.first_frame.offset;

	VFile *vfile = nullptr;
	auto &substor = storage[date];
	auto &files = substor[sequence.first_frame.camera];
	for (auto vf : files) {	
		if ((vf->Width() == sequence.width) && (vf->Height() == sequence.height)) {
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

	std::list<std::string> date_del;
	for (auto &substor_pair : storage) {

		std::list<size_t> cam_del;
		for (auto &files_pair : substor_pair.second) {

			files_pair.second.remove_if([this, sequence, max_distance](VFile *f)
				{
					assert(f != nullptr);
					assert(sequence.last_frame.offset >= f->LastFrameOffset());
					if ((sequence.first_frame.offset - f->LastFrameOffset()) >= max_distance
						|| (f->Size() >= max_file_size))
					{
						CloseVideoFile(*f);
						return true;
					} else {
						return false;
					}
				}
			);
			if (files_pair.second.size() == 0) {
				cam_del.push_back(files_pair.first);
			}
		}
		for (auto camera : cam_del) {
			substor_pair.second.erase(camera);
		}
		if (substor_pair.second.size() == 0) {
			date_del.push_back(substor_pair.first);
		}
	}
	for (auto &date : date_del) {
		storage.erase(date);
	}

	return true;
}