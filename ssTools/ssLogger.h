﻿//@ Project : ssTools
//@ Copyright : Non-unruly (Git)
//@ Git: https://github.com/Non-Unruly/ssTools
//@
//@ Launch Date : 2020-06-27

#ifndef _SS_LOGGER_H_
#define _SS_LOGGER_H_

#if defined _WIN32
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <cstring>
#include <string>
#include <vector>
#include <set>
#include <iterator>
#include <queue>
#include <cstdio>
#if defined _WIN32

#include <thread>
#include <mutex>


#else
// Linux
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>

#endif

#include "ssTools.h"

#define ssLoggerOpen(_path, _log_name, _level, _sync, _filesize) ssLogger::init(_path, _log_name, _level, _sync, _filesize)
#define ssLoggerFlags(flags) ssLogger::SetLogPrefix(flags)

#define ssloggerDebug(...) ssLogger::output(true, 1, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define ssloggerInfo(...) ssLogger::output(true, 2, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define ssloggerWarn(...) ssLogger::output(true, 3, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define ssloggerError(...) ssLogger::output(true, 4, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define ssloggerDisaster(...) ssLogger::output(true, 5, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define ssloggerDebug_np(...) ssLogger::output(false, 1, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define ssloggerInfo_np(...) ssLogger::output(false, 2, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define ssloggerWarn_np(...) ssLogger::output(false, 3, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define ssloggerError_np(...) ssLogger::output(false, 4, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define ssloggerDisaster_np(...) ssLogger::output(false, 5, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define PREFIX_TIME 0b00001	//时间戳
#define PREFIX_LEVEL 0b00010	//等级字符串
#define PREFIX_FILE 0b00100	//文件名
#define PREFIX_FUNC 0b01000	//函数名
#define PREFIX_LINE 0b10000	//行数
#define PREFIX_ALL 0b11111		//全部

class ssLogger
{
public:
	enum LOG_LEVEL
	{
		LEVEL_ALL,
		LEVEL_DBG,
		LEVEL_INF,
		LEVEL_WAR,
		LEVEL_ERR,
		LEVEL_DISASTER,
	};

	ssLogger();

	//日志模块初始化函数 log module initialize function
	// logPath 日志目录 log file path (auto craete dir)
	// name 日志文件名 log file's name
	// maxLen 单条日志最大长度 one log record string's length
	// level 日志精准等级[0~5] 等级越高，输出的日志越精确，日志量越少 the level of log , high level and fewer log and more accurate log
	// tag 日志输出的前缀字段，时间（精确至毫秒）、日志等级、触发文件、触发函数、触发行号 the log prefix about time(ms),level,file,func,line number
	// sync 同步日志开关 the sync/async mode of log , default is sync(true)
	static bool init(const char *_logPath, const char *_name, LOG_LEVEL _level = LEVEL_ALL, bool sync = true, unsigned int logFileSize = 256 * 1024 * 1024);

	// 添加日志字符串
	// print 是否输出到屏幕 whether the text is displayed on the screen
	// type 日志类型映射为等级 log's type/level (enum LOG_LEVEL)
	// functionName 当前所在函数的函数名 log's function name
	// line 日志点行数 log's line number
	// format 日志格式 format , support %d %ld %s %c %f %lf(%.8lf) %x(0x%02x)
	// return log text without preffix
	static std::string output(bool print, int type, const char *srcName, const char *functionName, int line, const char *format, ...);

	// 设置日志的前缀
	static void SetLogPrefix(int flags);

	typedef struct _SSLOG_INFO_T_
	{
		std::string prefix;
		bool isPrefix;//是否显示前缀
		std::string text;
		std::string log;

		std::string time;
		std::string type;
		std::string file;
		std::string function;
		int line;

		bool isPrint;
		_SSLOG_INFO_T_()
		{
			isPrint = false;
		};
		_SSLOG_INFO_T_(std::string _prefix, std::string _text, bool _isPrint, bool _isPrefix = true)
		{
			prefix = _prefix;
			text = _text;
			isPrint = _isPrint;
			isPrefix = _isPrefix;
			log = prefix + text + "\n";
		};
		//_SSLOG_INFO_T_(std::string _timetick, std::string _logType, std::string _file, std::string _function, int _line, std::string _logText, bool print = true)
		//{
		//	time = _timetick;
		//	file = _file;
		//	function = _function;
		//	line = _line;
		//	type = _logType;
		//	log = _logText;
		//	isPrint = print;
		//};
	} ssLog_info_t;

private:
	static FILE *m_f;
	static bool m_create_logfile();

	static std::string m_fileBaseName;
	static std::string m_fileName;
	static std::string m_logPath;
	static bool m_isInit;
	static bool m_sync;
	static int m_flags;
	static std::vector<std::string> m_level_map;
	static unsigned int m_file_max_size;
	static unsigned int m_file_sum;

#if defined _WIN32
	static std::mutex m_qMtx;
#else
	static pthread_mutex_t m_qMtx;
#endif
	static std::queue<ssLogger::ssLog_info_t> m_logQueue;
	static std::set<std::string> m_keys;

	static std::string ssFormat(int length, const char *_format, va_list _vlist);

#if defined _WIN32
	static void m_LogThread();
#else
	static void *m_LogThread(void *arg);
#endif
	static LOG_LEVEL m_logLevel;

#if defined _WIN32
	static void ss_lock(std::mutex &_mtx);
	static void ss_unlock(std::mutex &_mtx);
#else
	static void ss_lock(pthread_mutex_t &_mtx);
	static void ss_unlock(pthread_mutex_t &_mtx);
#endif
};

#endif // !_SS_LOGGER_H_
