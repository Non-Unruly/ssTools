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
#include <cstdio>

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>

#include "ssTools.h"

#define sslogDebug(...) ssLogger::output(true,1,__FUNCTION__,__LINE__,__VA_ARGS__)
#define sslogInfo(...) ssLogger::output(true,2,__FUNCTION__,__LINE__,__VA_ARGS__)
#define sslogWarn(...) ssLogger::output(true,3,__FUNCTION__,__LINE__,__VA_ARGS__)
#define sslogError(...) ssLogger::output(true,4,__FUNCTION__,__LINE__,__VA_ARGS__)
#define sslogDisaster(...) ssLogger::output(true,5,__FUNCTION__,__LINE__,__VA_ARGS__)

#define sslogDebug_np(...) ssLogger::output(false,1,__FUNCTION__,__LINE__,__VA_ARGS__)
#define sslogInfo_np(...) ssLogger::output(false,2,__FUNCTION__,__LINE__,__VA_ARGS__)
#define sslogWarn_np(...) ssLogger::output(false,3,__FUNCTION__,__LINE__,__VA_ARGS__)
#define sslogError_np(...) ssLogger::output(false,4,__FUNCTION__,__LINE__,__VA_ARGS__)
#define sslogDisaster_np(...) ssLogger::output(false,5,__FUNCTION__,__LINE__,__VA_ARGS__)

class ssLogger
{
public:
	enum LOG_LEVEL
	{
		ALL,
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		DISASTER,
	};

	ssLogger();

	//��־ģ���ʼ������ log module initialize function
	//logPath ��־�ļ�·�����Զ����.log��׺���� log file path (auto to add ".log" file suffix)
	//fileSize ��־�ļ���С��д���󣬴����µ���־�ļ�������־�ļ��������Զ����Ӵ���ʱ�����׺��log file's max size . if out of range , create new log file and old file will rename append timetick string
	//level ��־��׼�ȼ�[0~5] �ȼ�Խ�ߣ��������־Խ��ȷ����־��Խ�� the type/level of log , high level and fewer log and more accurate log
	static bool init(const char *logPath, size_t fileSize, LOG_LEVEL level = ALL);

	//�����־�ַ���
	//print �Ƿ��������Ļ whether the text is displayed on the screen
	//type ��־����ӳ��Ϊ�ȼ� log's type/level (enum LOG_LEVEL)
	//functionName ��ǰ���ں����ĺ����� log's function name
	//line ��־������ log's line number
	//format ��־��ʽ format , support %d %ld %s %c %f %lf(%.8lf) %x(0x%02x)
	static void output(bool print, int type, const char * functionName, int line, const char* format, ...);

	typedef struct _KEY_T_
	{
		std::string tagName;//�ؼ���
		int keylen;//�ؼ��ʳ���
		int pos;//�ؼ�����ʼλ��
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
		_SSLOG_INFO_T_(std::string timetick, std::string logType, std::string logText, bool print = true)
		{
			time = timetick;
			type = logType;
			log = logText;
			isPrint = print;
		}
	}ssLog_info_t;

private:
	static bool isInit;
	static size_t size;
	static std::queue<ssLogger::ssLog_info_t> logQueue;
	static std::set<std::string> keys;

	static std::string ssFormat(const char *format, va_list vlist);
	static std::string function_line(const char *function, int line);

	static void *LogThread(void *arg);

	static std::string logNamePrefix;
	static size_t logFileSize;
	static LOG_LEVEL logLevel;
};

#endif // !_SS_LOGGER_H_




