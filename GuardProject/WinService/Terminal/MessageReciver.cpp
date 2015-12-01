#include "MessageReciver.h"

MessageReciver::~MessageReciver()
{
	if (com_port) {
		delete com_port;
	}
}

bool MessageReciver::WaitForMessage(const char *com_name)
{
	if (com_port) {
		com_port->Close();
	}
	com_port = new ComPort(com_name);
	if (com_port && com_port->Open()) {
		start();
		return true;
	}
	return  false;
}

void MessageReciver::run()
{
	Message msg;
	BYTE *ptr = (BYTE *)&msg;
	int bytes_count = 0;
	BYTE byte;

	while (com_port->WaitForInputData()) {
		while (com_port->Read(&byte, 1)) {

			if (bytes_count == 0x00) {
				ptr = (BYTE *)&msg;
				if (byte == MESSAGE_HEADER) {
					ptr[bytes_count] = byte;
					bytes_count++;
				}
			} else {
				ptr[bytes_count] = byte;
				bytes_count++;
				if (bytes_count == sizeof(Message)) {
					if (msg.IsValidMessage()) {
						emit(MessageRecived(msg));
					}
					bytes_count = 0;
				}		
			}
		}
	}
}
