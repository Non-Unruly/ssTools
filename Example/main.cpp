#include <cstdio>
#include <iostream>
#include "../ssTools/ssLogger.h"
#include "../ssTools/ssNetwork.h"

#ifdef _WIN32
#include <Windows.h>
#else
#endif

int main()
{
	std::cout << __cplusplus << std::endl;
#if defined(_WIN32) || __cplusplus >= 201103L
	std::cout << "c++11" << std::endl;
#endif
	ssLogger::Register("./log", "A", "A");
	ssLogger::Register("./log", "B", "B");
	ssLogger::Register("./log", "root", "", ssLogger::LEVEL_ALL, false, 256 * 1024);
	sslogger().debug("sslogger launch %s !!!", ssTools::ss_datetime_simple().c_str());
	for (int i = 0; i < 100; i++)
	{
		sslogger("A").debug("hello world sslogger %s %d", "A hbshi3", i);
		sslogger("B").info("hello world sslogger %s %d", "B hbshi3", i);
		sslogger().error("hello word %s", "no module name %d", i);
#ifndef _WIN32
		usleep(1 * 1000 * 1000);
#else
		Sleep(100);
#endif
	}

	system("pause");
	return 0;
}