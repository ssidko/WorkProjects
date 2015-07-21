#include "stdafx.h"
#include "Error.h"

MyLib::Error::Error(Error &error)
{
	code = error.code;
	description = error.description;
}
