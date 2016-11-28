#include "G2fdbRecovery.h"
#include <sstream>
#include "utility.h"

using namespace G2FDB;

int G2FDB::StartRecovery(const std::string & dvr_volume, const std::string & out_directory, const dvr::Timestamp & start_time, const dvr::Timestamp & end_time)
{
	G2fdbVolume volume(dvr_volume); 
	VideoStorage storage;
	FrameSequence sequence;
	size_t max_delta_time = 60; // seconds

	if (volume.Open() && storage.Open(out_directory)) {

		sequence.Clear();
		sequence.data.reserve(MAX_FRAME_SEQUANCE_SIZE);

		//volume.SetPointer(500449675890LL);

		while (volume.ReadFrameSequence(sequence, max_delta_time)) {

			storage.SaveFrameSequence(sequence);
			sequence.Clear();
		}

		return 0;
	}

	return -1;
}

//
//	class VideoStorage
//

bool G2FDB::VideoStorage::CreateDirectory(const std::string &directory)
{
	bool result = ::CreateDirectoryA(directory.c_str(), NULL);

	if (!result) {
		if (ERROR_ALREADY_EXISTS == ::GetLastError()) {
			result = true;
		}
	}

	return result;
}

bool G2FDB::VideoStorage::Open(const std::string & storage_directory)
{
	base_directory = storage_directory;

	while (base_directory.size()) {
		char ch = base_directory.back();
		if (isspace(ch)) {
			base_directory.pop_back();
		} else {
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

bool G2FDB::VideoStorage::SaveFrameSequence(FrameSequence & sequence)
{
	// 2016-05-28
	char date[16] = {0};
	sprintf_s(date, sizeof(date), "%04u-%02u-%02u", sequence.start_time.year, sequence.start_time.month, sequence.start_time.day);

	std::string date_dir = base_directory + date + "\\";
	std::string cam_dir = date_dir + "CAM-" + std::to_string(sequence.camera) + "\\";
	std::string temp_file_name = cam_dir + "dvr.tmp";

	if (CreateDirectory(date_dir) && CreateDirectory(cam_dir)) {
		
		W32Lib::FileEx temp_file(temp_file_name.c_str());
		if (temp_file.Create()) {
			
			if (temp_file.Write(&sequence.data[0], sequence.data.size())) {

				std::stringstream file_name;
				file_name << cam_dir << "[" << sequence.camera << "]-";
				file_name << sequence.start_time.ToString() << "-=-" << sequence.end_time.ToString();
				file_name << "-=-" << std::to_string(sequence.offset);
				file_name << ".avi";

				Convert2Avi(temp_file_name, file_name.str());

				return true;			
			}
		}	
	}

	return false;
}

//
//	class VideoFile
//