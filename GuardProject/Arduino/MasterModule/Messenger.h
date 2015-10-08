#ifndef _MESSENGER_H
#define _MESSENGER_H

#include "Arduino.h"

#define MESSAGE_HEADER      (word)0x55AA
#define MESSAGE_FOOTER      (word)0xAA55

typedef struct _Message {
  word header;
  byte code;
  byte subcode;
  word footer;
} Message;

class Messenger
{
private:
  Stream *serial;
public:
  Messenger(Stream *serial_io) : serial(serial_io) {}
  ~Messenger(void) {}
  bool ReciveMessage(Message &msg);
  bool SendMessage(Message &msg);   
};

#endif // _MESSENGER_H
