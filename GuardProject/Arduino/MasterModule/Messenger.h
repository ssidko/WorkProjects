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
  Serial_ *serial;
public:
  Messenger(Serial_ *serial_port) : serial(serial_port) {}
  ~Messenger(void) {}
  void Begin();
  bool ReciveMessage(Message &msg);
  bool SendMessage(Message &msg);   
};

#endif // _MESSENGER_H
