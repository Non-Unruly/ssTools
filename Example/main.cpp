#include <cstdio>
#include "../ssFormatPlus.h"
#include <iostream>
#include <vector>
#include <list>
#include <string>

int main()
{
	std::vector<int> nums = { 1,2,3,4,5 };
	std::vector<std::string> stdstrs = {"hbshi","hbshi3","weiwang70"};
	std::vector<const char*> strs = { "what","do","I","want" };

	printf("%s\n", ssTools::ssFormatVecInt(nums).c_str());
	printf("%s\n", ssTools::ssFormatVecStdStr(stdstrs).c_str());
	printf("%s\n", ssTools::ssFormatVecStr(strs).c_str());
	system("pause");
	return 0;
}