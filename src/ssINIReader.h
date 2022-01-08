//@ Project : ssTools
//@ Copyright : Non-unruly (Git)
//@ Git: https://github.com/Non-Unruly/ssTools
//@
//@ Launch Date : 2020-06-27
//@ Update: 2020-12-15

#ifndef _SS_INIREADER_H_
#define _SS_INIREADER_H_

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <vector>

class ssINIReader
{
public:
	static ssINIReader *instance();

	bool Load(const char *_iniFilePath);

	std::string GetStrValue(std::string _root, std::string _key);
	int GetIntValue(std::string _root, std::string _key);
	float GetFltValue(std::string _root, std::string _key);
	bool GetBoolValue(std::string _root, std::string _key);

private:
	ssINIReader();
	~ssINIReader();
	static ssINIReader *inst;

	std::string errmsg;
	std::map<std::string, std::map<std::string, std::string>> ini;

	std::string trim(std::string str);
};

#endif