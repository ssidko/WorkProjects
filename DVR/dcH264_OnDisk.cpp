#include "dcH264_OnDisk.h"
#include <assert.h>

bool dcH264::FRAME_HEADER::IsValid(void)
{
	if ((camera < '0') || (camera > '9')) {
		return false;
	}

	if ((subtype != '0') && (subtype != '1')) {
		return false;
	}

	switch (frame_type) {
	case 'cd':
		if (dc.signature != '462H') {
			return false;
		}
		break;
	case 'bw':
		break;
	default :
		return false;
	}

	return true;
}

size_t dcH264::FRAME_HEADER::HeaderSize(void)
{
	switch (frame_type) {
	case 'cd':
		switch (subtype) {
		case '0':
			return dc.subtype_0.kHeaderSize;
		case '1':
			return dc.subtype_1.kHeaderSize;
		default:
			return 0;
		}
	case 'bw':
		return wb.kHeaderSize;
	default:
		return 0;
	}
	return 0;
}

size_t dcH264::FRAME_HEADER::PayloadSize(void)
{
	switch (frame_type) {
	case 'cd':
		return dc.payload_size;
	case 'bw':
		return wb.size_1;
	default:
		return 0;
	}
	return 0;
}

size_t dcH264::FRAME_HEADER::FrameSize(void)
{
	assert(this->IsValid());
	return HeaderSize() + PayloadSize();
}

uint8_t *dcH264::FRAME_HEADER::Payload(void)
{
	assert(this->IsValid());
	return (uint8_t *)((ptrdiff_t)this + (ptrdiff_t)this->HeaderSize());
}
