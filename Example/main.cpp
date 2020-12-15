#include <cstdio>
#include "../ssTools.h"
#include "../ssLogger.h"
#include <iostream>
#include <Windows.h>

int main()
{
	bool isInit = ssLoggerOpen("d:\\shb", 1 * 1024 * 104, ssLogger::ALL);
	
	while (true)
	{
		ssloggerDebug("hello ssTools logger!");
		Sleep(500);
	}
    return 0;
}