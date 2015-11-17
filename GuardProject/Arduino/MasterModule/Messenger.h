#ifndef _MESSENGER_H
#define _MESSENGER_H

#include "Arduino.h"
#include "Stream.h"

#define MESSAGE_HEADER      (word)0x55AA
#define MESSAGE_FOOTER      (word)0xAA55

typedef struct _Message {
  word header;
  byte type;
  byte code;
  word footer;
} Message;

class Messenger
{
private:
  Stream *com_port;
  Message message;
  byte recived_bytes;
  void ClearMessage(Message &msg);
public:
  Messenger(void);
  ~Messenger(void) {}
  void Init(Stream &serial);
  bool IsValidMessage(Message &msg);
  bool ReciveMessage(Message &msg);
  bool SendMessage(Message &msg);   
};

#endif // _MESSENGER_H
