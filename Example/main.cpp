#include <cstdio>
#include <iostream>
#include "../src/ssLogger.h"

int main()
{
	ssLoggerOpen("./log/demo", "app", ssLogger::LEVEL_ALL, false);
	ssloggerDebug("%08d", 19910915);
	ssloggerInfo_np("%s", "ssTools");

	while (true)
	{
		ssloggerDebug("%s", ssTools::ss_datetime().c_str());
#ifndef _WIN32
		usleep(3 * 1000 * 1000);
#else
		Sleep(3 * 1000);
#endif
	}

	system("pause");
	return 0;
}