//@ Project : ssTools
//@ Copyright : Non-unruly (Git)
//@ Git: https://github.com/Non-Unruly/ssTools
//@
//@ Launch Date : 2020-06-27
//@ Update: 2021-01-24

#include "ssFormatPlus.h"

std::string ssTools::ssFormatVecInt(std::vector<int> _container)
{
	std::string result;

	result = "[";
	for (std::vector<int>::iterator iter = _container.begin(); iter != _container.end(); iter++)
	{
		char num[32];
		sprintf(num, "%d\0", *iter);
		result = result + num;
		iter != _container.end() - 1 ? (result += " , ") : result;
	}
	result += "]";
	return result;
}

std::string ssTools::ssFormatVecStdStr(std::vector<std::string> _container)
{
	std::string result;

	result = "[";
	for (std::vector<std::string>::iterator iter = _container.begin(); iter != _container.end(); iter++)
	{
		result = result + *iter;
		iter != _container.end() - 1 ? (result += " , ") :result;
	}
	result += "]";

	return result;
}

std::string ssTools::ssFormatVecStr(std::vector<const char*> _container)
{
	std::string result;

	result = "[";
	for (std::vector<const char*>::iterator iter = _container.begin(); iter != _container.end(); iter++)
	{
		result += std::string(*iter);
		iter != _container.end() - 1 ? (result += " , ") : result;
	}
	result += "]";

	return result;
}
