#include "MessageReciver.h"

#include <vector>

char *msg_types[] = { "kUnknownMessage", "kIdentification", "kNotification", "kCommand" };

char *notifications[] = { "kUnknownNotification", "kGuardModeEnabled", "kGuardModeDisabled", "kLine1Activated", "kLine2Activated",
	"kLine3Activated", "kLine4Activated", "kLine5Activated", "kAllLinesActivated", "kGsmEraseAllActivated", "kKeyEraseAllActivated",
	"kMasterGuardSensor1Activated", "kSlaveGuardSensor1Activated", "kLinkToSlaveLosted", "kLinkToMasterLosted" };

char *commands[] = { "kUnknownCommand", "kEnableGuardMode", "kDisableGuardMode", "kActivateLine1",
	"kActivateLine2", "kActivateLine3", "kActivateLine4", "kActivateLine5", "kActivateAllLines" };

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

std::string Message::TypeToString(void)
{
	std::string str = "Unknown";

	if (type < sizeof(msg_types)) {
		str = msg_types[type];
	}

	return str;
}

std::string Message::CodeToString(void)
{
	std::string str = "Unknown";
	switch (type) {
	case MessageType::kIdentification:
		str = std::to_string(code);
		break;
	case MessageType::kNotification:
		if (code < sizeof(notifications)) {
			str = notifications[code];
		}
		break;
	case MessageType::kCommand:
		if (code < sizeof(commands)) {
			str = commands[code];
		}
		break;
	default:
		break;	
	}

	return str;
}

std::string Message::ToString(void)
{
	std::string str = "MSG {";
	str += TypeToString() + ", ";
	str += CodeToString();
	str += "};";
	return str;
}
