#include "Messenger.h"

Messenger::Messenger()
{
  recived_bytes = 0;  
}

void Messenger::ClearMessage(Message &msg)
{
  memset(&msg, 0x00, sizeof(Message));  
}

void Messenger::Init(Stream &serial)
{
  com_port = &serial;
  ClearMessage(message);
}

bool Messenger::IsValidMessage(Message &msg)
{
  return ((msg.header == MESSAGE_HEADER) && (msg.footer == MESSAGE_FOOTER));  
}

bool Messenger::ReciveMessage(Message &msg)
{
  char *incoming_byte;
  
  if (com_port) {
    while (Serial.available()) {
      
      ((char *)&message)[recived_bytes] = Serial.read();

      recived_bytes++;

      if (recived_bytes >= sizeof(Message)) {
        recived_bytes = 0;
        if (IsValidMessage(message)) {
          msg = message;          
          
          
        }
      }
    }
  }
  return false;
}

bool Messenger::SendMessage(Message &msg)
{
  if (com_port) {
    return com_port->write((const char *)&msg, sizeof(Message)) == sizeof(Message) ? true : false;
  }
  return false;  
}

