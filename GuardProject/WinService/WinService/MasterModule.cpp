#include "stdafx.h"
#include "MasterModule.h"

MasterModule::MasterModule() : last_error(0), opened(false)
{
}

MasterModule::~MasterModule()
{
}

bool MasterModule::Identify(void)
{
	Message id_cmd(MessageType::kIdentification, 0);
	Message answer;
	if (com.Write(&id_cmd, sizeof(Message))) {
		if (WaitForMessage(answer, kIdentificationAnswerTimeout)) {
			if ((answer.type == MessageType::kIdentification) && (answer.code == IdentificationType::kMasterID)) {
				return true;
			}
		}	
	}
	return false;	
}

bool MasterModule::Open(const std::string &com_name)
{
	if (opened) {
		this->Close();
	}
	if (com.Open(com_name.c_str())) {
		if (Identify()) {
			opened = true;
			return true;
		} else {
			this->Close();
		}
	}
	return false;
}

void MasterModule::Close()
{
	com.Close();
	last_error = 0;
	opened = false;
}

bool MasterModule::WaitForMessage(Message &msg)
{
	BYTE *ptr = (BYTE *)&msg;
	int bytes_count = 0;
	BYTE byte;

	GetTickCount();

	msg.Clear();
	while (com.WaitForInputData()) {
		while (com.Read(&byte, 1)) {

			if (bytes_count == 0x00) {
				ptr = (BYTE *)&msg;
				if (byte == MESSAGE_HEADER) {
					ptr[bytes_count] = byte;
					bytes_count++;
				}
			}
			else {
				ptr[bytes_count] = byte;
				bytes_count++;
				if (bytes_count == sizeof(Message)) {
					if (msg.IsValidMessage()) {
						//
						// Message recived.
						//
						return true;
					}
					bytes_count = 0;
				}
			}
		}
	}
	msg.Clear();
	return false;
}

bool MasterModule::WaitForMessage(Message &msg, DWORD timeout)
{
	BYTE *ptr = (BYTE *)&msg;
	int bytes_count = 0;
	BYTE byte;
	DWORD remain_time = timeout;

	DWORD start_time = ::GetTickCount();

	while (com.WaitForInputData(remain_time)) {
		while (com.Read(&byte, 1)) {

			if (bytes_count == 0x00) {
				ptr = (BYTE *)&msg;
				if (byte == MESSAGE_HEADER) {
					ptr[bytes_count] = byte;
					bytes_count++;
				}
			}
			else {
				ptr[bytes_count] = byte;
				bytes_count++;
				if (bytes_count == sizeof(Message)) {
					if (msg.IsValidMessage()) {
						//
						// Message recived.
						//
						return true;
					}
					bytes_count = 0;
				}
			}

		}
		remain_time = ::GetTickCount() - start_time;
		if (remain_time > timeout) {
			break;
		}
	}
	msg.Clear();
	return false;
}
