#include "ssException.h"

ssException::ssException()
{
	no = 0;
	msg = "no error";
}

ssException::ssException(int code, const char * msg)
{
}

ssException::~ssException()
{
}

std::string ssException::error()
{
	return msg;
}

int ssException::code()
{
	return no;
}

const char *ssException::output()
{
	char *text = new char[msg.length() + 32];
	memset(text, 0, msg.length() + 32);
	sprintf(text, "%s , code is %d\0", msg.c_str(), no);
	
	std::string plainText(msg);
	
	delete[]text;

	return plainText.c_str();
}