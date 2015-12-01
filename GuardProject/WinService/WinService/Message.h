#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "MessageTypes.h"
#include <windows.h>

#define MESSAGE_HEADER      (BYTE)0x55
#define MESSAGE_FOOTER      (BYTE)0xAA

#pragma pack (push)
#pragma pack (1)

struct Message {
	BYTE header;
	BYTE type;
	BYTE code;
	BYTE footer;

	bool IsValidMessage(void) {
		return ((header == MESSAGE_HEADER) && (footer == MESSAGE_FOOTER));
	}
	void Clear(void) {
		memset(this, 0x00, sizeof(Message));
		header = MESSAGE_HEADER;
		footer = MESSAGE_FOOTER;
	}
	Message() {
		header = MESSAGE_HEADER;
		footer = MESSAGE_FOOTER;
		type = 0;
		code = 0;
	}
	Message(byte msg_type, byte msg_code) {
		header = MESSAGE_HEADER;
		footer = MESSAGE_FOOTER;
		type = msg_type;
		code = msg_code;
	};
};

#pragma pack(pop)

#endif // _MESSAGE_H