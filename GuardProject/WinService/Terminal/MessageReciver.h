#pragma once

#include <QObject>
#include <qthread.h>
#include "ComPort.h"

#include "MessageTypes.h"

#define MESSAGE_HEADER      (BYTE)0x55
#define MESSAGE_FOOTER      (BYTE)0xAA

#pragma pack (push)
#pragma pack (1)

typedef struct _Message {
	BYTE header;
	BYTE type;
	BYTE code;
	BYTE footer;

	bool IsValidMessage(void) {
		return ((header == MESSAGE_HEADER) && (footer == MESSAGE_FOOTER));
	}
	void Clear(void) {
		memset(this, 0x00, sizeof(_Message));
		header = MESSAGE_HEADER;
		footer = MESSAGE_FOOTER;
	}
	_Message() {
		header = MESSAGE_HEADER;
		footer = MESSAGE_FOOTER;
		type = 0;
		code = 0;
	}
	_Message(byte msg_type, byte msg_code) {
		header = MESSAGE_HEADER;
		footer = MESSAGE_FOOTER;
		type = msg_type;
		code = msg_code;
	};
} Message;

#pragma pack(pop)

class MessageReciver : public QThread
{
	Q_OBJECT
private:
	QThread thread;
	ComPort *com_port;
protected:
	virtual void run();
public:
	MessageReciver(void) : com_port(nullptr) {}
	~MessageReciver();

	bool WaitForMessage(const char *com_name);

signals:
	void MessageRecived(Message);

};

