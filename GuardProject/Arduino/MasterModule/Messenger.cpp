#include "Messenger.h"

void Messenger::Begin(long speed)
{
  serial->begin(speed);
}

bool Messenger::ReciveMessage(Message &msg)
{
//  if (serial) {
    serial->println("Test call: Messenger::ReciveMessage()");
//  }
  return false;
}

bool Messenger::SendMessage(Message &msg)
{
//  if (serial) {
    serial->println("Test call: Messenger::SendMessage()");
//  }
  return false;  
}

