#include "dhfs.h"

int DHFS::StartRecovering( void )
{
	Volume dhfs("\\\\.\\PhysicalDrive1");
	if (dhfs.Open()) {
		std::vector<BYTE> sequence;
		FrameSequenceInfo sequnence_info;
		sequnence_info.Clear();

		while (dhfs.NextFrameSequence(sequence, sequnence_info)) {
			sequence.clear();
			int x = 0;		
		}	
	}

	return 0;
}
