#define WIN32_LEAN_AND_MEAN
#include <cstdio>
#include <iostream>
#include "../ssTools/ssLogger.h"
#include "../ssTools/ssNetwork.h"
// #include "../ssTools/ssArgs.h"

#ifdef _WIN32
#include <Windows.h>
#else
#endif

int main(int argc, char **argv)
{
	// ssArgs args;
	// args.addRuler<int>("name", 'n', "name string", true, 123);

	std::cout << __cplusplus << std::endl;
#if defined(_WIN32) || __cplusplus >= 201103L
	std::cout << "c++11" << std::endl;
#endif
	ssLogger::Register("./log", "A", "A");
	ssLogger::Register("./log", "B", "B");
	ssLogger::Register("./log", "root", "", ssLogger::LEVEL_ALL, false, 256 * 1024);
	ss().loggerDebug("sslogger launch %s !!!", ssTools::ss_datetime().c_str());
	// ss().lo("sslogger launch %s !!!", ssTools::ss_datetime_simple().c_str());
	for (int i = 0; i < 100; i++)
	{
		ss("A").loggerDebug("hello world sslogger %s %d", "A hbshi3", i);
		ss("B").loggerInfo("hello world sslogger %s %d", "B hbshi3", i);
		ss().loggerWarn("hello word %s %d", "no module name", i);
		ss("C").loggerInfo("this is no register log , default %02x", i);
#ifndef _WIN32
		usleep(1 * 1000 * 1000);
#else
		Sleep(100);
#endif
	}

	system("pause");
	return 0;
}