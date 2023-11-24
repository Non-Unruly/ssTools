#include <cstdio>
#include <iostream>
#include "../ssTools/ssLogger.h"
#include "../sstools/ssNetwork.h"

#ifdef _WIN32
#include <Windows.h>
#else
#endif

int main()
{
	
	ssLoggerOpen("./log/example", "app", ssLogger::LEVEL_ALL, false, 5 * 1024 * 1024);
	ssLoggerFlags(PREFIX_TIME | PREFIX_FUNC | PREFIX_LINE | PREFIX_FILE);
	ssloggerDebug("%08d", 19910915);
	ssloggerInfo_np("%s", "ssTools");
	std::string ip;
	auto ips = ssTools::GetLocalIPv4(ip);
	ssloggerInfo("local ip: %s", ip.c_str());


	while (true)
	{
		ssloggerDebug("%s", ssTools::ss_datetime().c_str());
#ifndef _WIN32
		usleep(3 * 1000 * 1000);
#else
		Sleep(300);
#endif
	}

	system("pause");
	return 0;
}