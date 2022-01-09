#include <cstdio>
#include <iostream>
#include "../src/ssLogger.h"

#include "../src/ssLogger.h"

int main()
{
	ssLoggerOpen("./log/demo", "app", 5 * 1024 * 1024, ssLogger::LEVEL_ALL, false);
	ssloggerDebug("%08d", 19910915);
	ssloggerInfo_np("%s", "ssTools");
#ifndef _WIN32
	while (true)
	{
		ssloggerDebug("%s", ssTools::ss_datetime().c_str());
		usleep(3 * 1000);
	}
#endif
	return 0;
}