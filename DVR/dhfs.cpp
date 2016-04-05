#include "dhfs.h"

int DHFS::StartRecovering( void )
{
	Volume dhfs("\\\\.\\PhysicalDrive1");
	if (dhfs.Open()) {
		std::vector<BYTE> sequence;
		FrameSequenceInfo sequnence_info;
		FileStorage storage(32, "F:\\39405\\raw\\", "F:\\39405\\mkv\\");


		while (dhfs.NextFrameSequence(sequence, sequnence_info)) {
			storage.SaveFrameSequence(sequence, sequnence_info);
			sequence.clear();
			sequnence_info.Clear();
			int x = 0;		
		}	
	}

	return 0;
}

int DHFS::StartRecovering(const std::string &dhfs_volume, const std::string &out_directory, const Timestamp &start_time, const Timestamp & end_time)
{
	Volume volume(dhfs_volume);
	if (volume.Open()) {
	
		std::vector<BYTE> sequence;
		FrameSequenceInfo sequence_info;
		FileStorage storage(32, out_directory, "");


		while (volume.NextFrameSequence(sequence, sequence_info)) {

			if (start_time.Seconds()) {
				if (sequence_info.start_frame.timestamp.Seconds() < start_time.Seconds()) {
					if (sequence_info.end_frame.timestamp.Seconds() < start_time.Seconds()) {
						continue;
					}
				}				
			}

			if (end_time.Seconds()) {
				if (sequence_info.start_frame.timestamp.Seconds() > end_time.Seconds()) {
					continue;
				}				
			}


			storage.SaveFrameSequence(sequence, sequence_info);
			sequence.clear();
			sequence_info.Clear();
			int x = 0;
		}
	
	}




	return 0;
}
