#include "Messenger.h"

void Messenger::Init(Stream &serial)
{
  com_port = &serial;
}

bool Messenger::ReciveMessage(Message &msg)
{
  msg.Clear();
  if (com_port) {
    if (com_port->available()) {
      int readed_bytes = com_port->readBytes((char *)&msg, sizeof(Message));
      if (sizeof(Message) == readed_bytes) {
        return msg.IsValidMessage();  
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

