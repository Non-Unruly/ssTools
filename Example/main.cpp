#include <cstdio>
#include <iostream>
#include "../ssLogger.h"

#include "../ssFormatPlus.h"
#include "../ssLogger.h"


int main()
{
	ssLogger::init("test", 1024, 1024, ssLogger::LEVEL_ALL, ssLogger::LOGTAG_ALL, false);
	ssloggerDebug("%08d", 19910915);
	ssloggerInfo_np("%s", "ssTools");
	system("pause");
	return 0;
}