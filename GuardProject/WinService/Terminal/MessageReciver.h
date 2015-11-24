#pragma once

#include <QObject>
#include <qthread.h>
#include "ComPort.h"

#include "MessageTypes.h"

#define MESSAGE_HEADER      (byte)0x55
#define MESSAGE_FOOTER      (byte)0xAA

#pragma pack (push)
#pragma pack (1)

typedef struct _Message {
	byte header;
	byte type;
	byte code;
	byte footer;

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
	_Message(byte msg_type, byte code) {
		header = MESSAGE_HEADER;
		footer = MESSAGE_FOOTER;
	};
} Message;

#pragma pack(pop)

class MessageReciver : public QThread
{
	Q_OBJECT
private:
	QThread thread;
	ComPort com_port;
protected:
	virtual void run();
public:
	MessageReciver(const char *com_name) : com_port(com_name) {}
	~MessageReciver();

	void WaitForMessage(ComPort &com_port);

signals:
	void MessageRecived(Message);

};

