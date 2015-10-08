#include "Messenger.h"

bool Messenger::ReciveMessage(Message &msg)
{
  serial->println("Test call: Messenger::ReciveMessage()");
  return false;
}

bool Messenger::SendMessage(Message &msg)
{
  serial->println("Test call: Messenger::SendMessage()");
  return false;  
}
