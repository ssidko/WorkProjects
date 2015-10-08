#include "Messenger.h"

void Messenger::Begin(void)
{
  serial->begin(9600);
}

bool Messenger::ReciveMessage(Message &msg)
{
  if (*serial) {
    serial->println("Test call: Messenger::ReciveMessage()");
  }
  return false;
}

bool Messenger::SendMessage(Message &msg)
{
  if (*serial) {
    serial->println("Test call: Messenger::SendMessage()");
  }
  return false;  
}
