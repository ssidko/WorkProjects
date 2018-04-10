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

int G2FDB::StartRecovery(const dvr::RecoveryTask &task)
{	
	G2fdbVolume volume(task.io_name);
	dvr::VideoStorage storage(task.output_dir);
	dvr::FrameSequence sequence;

	const size_t max_sequence_size = 100 * 1024 * 1024;
	const size_t max_delta_time = 1;

	if (volume.Open() && storage.Open()) {
		
		sequence.Clear();
		sequence.buffer.reserve(max_sequence_size);
		volume.SetPointer(task.io_offset);

		try {

			while (volume.ReadFrameSequence(sequence, max_sequence_size, max_delta_time)) {

				storage.SaveFrameSequence(sequence);
				sequence.Clear();
			}
			return 0;

		} catch (std::system_error &exc) {
			std::string error_description = exc.what();
			return -1;
		}

}
	return -1;
}

void G2FDB::TestRecovery(void)
{
	dvr::RecoveryTask task;
	task.io_name = "\\\\.\\PhysicalDrive0";
	//task.io_offset = 2165338112ull;
	task.io_offset = 21495808ull * 512;
	task.io_size = 1953523119ull * 512;
	task.output_dir = "F:\\43889\\test\\   ";

	G2FDB::StartRecovery(task);
}


void G2FDB::TestRecovery2(void)
{
	const size_t unit_size = 256 * 1024 * 1024;
	const size_t descriptors_offset = 0x100000;
	const size_t data_offset = 0x500000;
	const size_t descriptors_count = (data_offset - descriptors_offset) / sizeof(frame_descriptor_t);

	size_t unit_num = 0;

	W32Lib::FileEx io("d:\\work\\g2fdb-dvr\\chunk-2");
	if (io.Open()) {

		std::vector<frame_descriptor_t> descriptors(descriptors_count);

		io.SetPointer((uint64_t)unit_num * unit_size + descriptors_offset);

		size_t readed = 0;
		size_t prev_cam_id = 0;
		size_t prev_offset = 0;
		size_t frames_count = 0;
		size_t used_space = 0;
		readed = io.Read((uint8_t *)descriptors.data(), descriptors.size() * sizeof(frame_descriptor_t));

		for (auto &descriptor : descriptors) {
			
			
			
			if (descriptor.signature == FRAME_HEADER_SIGNATURE) {

				FRAME_HEADER header = { 0 };

				io.SetPointer((uint64_t)unit_num * unit_size + data_offset + descriptor.frame_offset);
				readed = io.Read((uint8_t *)&header, sizeof(FRAME_HEADER));
				
				if ((header.signature == FRAME_HEADER_SIGNATURE) &&
					descriptor.frame_offset == header.frame_offset) {

					frames_count++;
					used_space += header.payload_size;

					if (prev_cam_id != descriptor.camera_id) {
						int x = 0;
					}

					prev_cam_id = descriptor.camera_id;
					prev_offset = descriptor.frame_offset;
				
				} else {
					int x = 0;
				}


			} else {
				int x = 0;
			}


		}

		int x = 0;


	}
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
