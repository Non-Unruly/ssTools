#ifndef _SS_LOGGER_H_
#define _SS_LOGGER_H_

#ifdef WIN32
	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS	
	#endif // !_CRT_SECURE_NO_WARNINGS
#endif // WIN32

#include <iostream>
#include <stdarg.h>
#include <cstring>
#include <string>
#include <vector>
#include <set>
#include <iterator>
#include <queue>

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>

typedef struct _KEY_T_
{
	std::string tagName;//关键词
	int keylen;//关键词长度
	int pos;//关键词起始位置
	std::string txt;
	_KEY_T_()
	{
		tagName = "";
		keylen = 0;
		pos = -1;
		txt = "";
	};
	_KEY_T_(std::string tag, int len, int pos_t)
	{
		tagName = tag;
		keylen = len;
		pos = pos_t;
	};
}KEY_T;

typedef struct _SSLOG_INFO_T_
{
	std::string time;
	std::string type;
	std::string log;
	bool isPrint;
	_SSLOG_INFO_T_()
	{
		isPrint = false;
	};
	_SSLOG_INFO_T_(std::string timetick,std::string logType,std::string logText, bool print=true)
	{
		time = timetick;
		type = logType;
		log = logText;
		isPrint = print;
	}
}ssLog_info_t;

class ssLogger
{
public:
	static bool init(const char *logPath, size_t fileSize);

	static void debug(const char* format, ...);
	static void info(const char* format, ...);
	static void warning(const char* format, ...);
	static void error(const char* format, ...);
	static void disaster(const char* format, ...);
	
	static void debugp(const char* format, ...);
	static void infop(const char* format, ...);
	static void warningp(const char* format, ...);
	static void errorp(const char* format, ...);
	static void disasterp(const char* format, ...);

private:
	static size_t size;
	static std::queue<ssLog_info_t> logQueue;
	static std::set<std::string> keys;

	static std::string ssFormat(const char *format, va_list vlist);
	//static void insert(ssLog_info_t t);

	static void *LogThread(void *arg);
	
	static std::string logNamePrefix;
	static size_t logFileSize;
public:
	//static void out();
};

#endif // !_SS_LOGGER_H_



