#include "dhfs.h"

int DHFS::StartRecovering( void )
{
	Volume dhfs("\\\\.\\PhysicalDrive2");
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
