//@ Project : ssTools
//@ Copyright : Non-unruly (Git)
//@ Git: https://github.com/Non-Unruly/ssTools
//@
//@ Launch Date : 2020-06-27

#ifndef _SS_EXCEPTION_H_
#define _SS_EXCEPTION_H_

#include <iostream>
#include <string>
#include <exception>
#include <string.h>
#include <stdio.h>

class ssException:
	public std::exception
{
public:
	ssException();

	ssException(int code, const char *msg) :no(code), msg(msg) {};

	~ssException();

	std::string error();
	int code();

	const char* output();

private:
	std::string msg;
	int no;
};

#endif

