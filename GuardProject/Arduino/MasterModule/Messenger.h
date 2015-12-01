#ifndef _MESSENGER_H
#define _MESSENGER_H

#include "Arduino.h"
#include "Stream.h"

#define MESSAGE_HEADER      (byte)0x55
#define MESSAGE_FOOTER      (byte)0xAA

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
  _Message(byte msg_type, byte msg_code) {
    header = MESSAGE_HEADER;
    footer = MESSAGE_FOOTER;
    type = msg_type;
    code = msg_code;
  };
} Message;

class Messenger
{
private:
  Stream *com_port;
public:
  Messenger(void) {}
  ~Messenger(void) {}
  void Init(Stream &serial);
  bool ReciveMessage(Message &msg);
  bool SendMessage(Message &msg);   
};

#endif // _MESSENGER_H
