#include "MessageReciver.h"


//MessageReciver::MessageReciver()
//{
//}


MessageReciver::~MessageReciver()
{
}

void MessageReciver::WaitForMessage(ComPort &com_port)
{
	com = &com_port;
	this->start();
}

void MessageReciver::run()
{

}
