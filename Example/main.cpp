#include <cstdio>
#include <iostream>
#include "../ssLogger.h"

int main()
{
	ssLoggerOpen("./example", 1 * 1024 * 1024, 2048, ssLogger::LEVEL_ALL, ssLogger::TAG_ALL);

	for (int i = 0; i < 10; i++)
	{
		ssloggerDebug("log %d %s", i, "shb");
	}

	system("pause");
	return 0;
}