#include "stdafx.h"
#include "MasterModule.h"

MasterModule::MasterModule() : last_error(0)
{
}

MasterModule::~MasterModule()
{
}

bool MasterModule::Open(std::string &com_name)
{
	if (com.Open(com_name.c_str())) {
	
	
	} else {
		
	}
	return false;
}

void MasterModule::Close()
{
	com.Close();
	last_error = 0;
}

bool WaitForMessage(Message &msg)
{


	return false;
}
