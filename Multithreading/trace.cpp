#include "stdafx.h"
#include "trace.h"

#include <iostream>
#include <mutex>

std::mutex trace_mtx;

void trace(const std::string str)
{
	std::lock_guard<std::mutex> lock(trace_mtx);
	std::cout << str << std::endl;
}

