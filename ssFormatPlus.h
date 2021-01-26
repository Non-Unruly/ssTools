//@ Project : ssTools
//@ Copyright : Non-unruly (Git)
//@ Git: https://github.com/Non-Unruly/ssTools
//@
//@ Launch Date : 2020-06-27
//@ Update: 2021-01-24

#ifndef _SS_FORMATPLUS_H_
#define _SS_FORMATPLUS_H_

#include <iostream>
#include <string>
#include <string.h>
#include <list>
#include <vector>
#include <typeinfo>

namespace ssTools
{
	std::string ssFormatVecInt(std::vector<int> _container);

	std::string ssFormatVecStdStr(std::vector<std::string> _container);

	std::string ssFormatVecStr(std::vector<const char*> _container);
}

#endif // !_SS_FORMATPLUS_H_
