#include <cstdio>
#include <iostream>
#include "../src/ssLogger.h"

#include "../src/ssLogger.h"


int main()
{
	ssLoggerOpen("c:\\log", "app", 5 * 1024 * 1024, ssLogger::LEVEL_ALL, false);
	ssloggerDebug("%08d", 19910915);
	ssloggerInfo_np("%s", "ssTools");
	system("pause");
	return 0;
}