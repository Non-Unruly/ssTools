//@ Project : ssTools
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

#ifdef _WIN32

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

#define sslogger(_module) ssLogger::Logger(_module)
#define debug(...) Debug(false, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define info(...) Info(false, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define warn(...) Warn(false, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define error(...) Error(false, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define disaster(...) Disaster(false, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define PREFIX_TIME 0b00001	 // 时间戳
#define PREFIX_LEVEL 0b00010 // 等级字符串
#define PREFIX_FILE 0b00100	 // 文件名
#define PREFIX_FUNC 0b01000	 // 函数名
#define PREFIX_LINE 0b10000	 // 行数
#define PREFIX_ALL 0b11111	 // 全部

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

	// 日志模块注册函数 log module initialize function
	//  _logPath 日志目录 log file path (auto craete dir)
	//  _logFileName 日志文件名 log file's name , it's will append timestampe suffix，（会自动追加时间戳后缀）
	// _logModuelName 模块名称，不同模块会区分不同日志文件
	//  _level 日志精准等级[0~5] 等级越高，输出的日志越精确，日志量越少 the level of log , high level and fewer log and more accurate log
	//  sync 同步日志开关 the sync/async mode of log , default is sync(true)
	// _logFileSize 单个日志文件的上限大小
	static int Register(const char *_logPath, const char *_logFileName, const char *_logModuelName, LOG_LEVEL _level = LEVEL_ALL, bool _sync = true, unsigned int _logFileSize = 16 * 1024 * 1024);

	static ssLogger Logger(const char *module = "");

private:
	static std::vector<ssLogger*> m_loggers;

public:

	ssLogger(const char *_logPath, const char *_logFileName, const char *_logModuelName, LOG_LEVEL _level = LEVEL_ALL, bool _sync = true, unsigned int _logFileSize = 16 * 1024 * 1024);

	bool init(const char *_logPath, const char *_logFileName, const char *_logModuelName, LOG_LEVEL _level = LEVEL_ALL, bool sync = true, unsigned int logFileSize = 16 * 1024 * 1024);

	std::string Debug(bool print, const char *srcName, const char *funcName, int line, const char *format, ...);
	std::string Info(bool print, const char *srcName, const char *funcName, int line, const char *format, ...);
	std::string Warn(bool print, const char *srcName, const char *funcName, int line, const char *format, ...);
	std::string Error(bool print, const char *srcName, const char *funcName, int line, const char *format, ...);
	std::string Disaster(bool print, const char *srcName, const char *funcName, int line, const char *format, ...);

	// 添加日志字符串
	// print 是否输出到屏幕 whether the text is displayed on the screen
	// type 日志类型映射为等级 log's type/level (enum LOG_LEVEL)
	// functionName 当前所在函数的函数名 log's function name
	// line 日志点行数 log's line number
	// format 日志格式 format , support %d %ld %s %c %f %lf(%.8lf) %x(0x%02x)
	// return log text without preffix
	std::string output(bool print, int type, const char *srcName, const char *functionName, int line, std::string text);

	std::string GetModName();

	typedef struct _SSLOG_INFO_T_
	{
		std::string prefix;
		bool isPrefix; // 是否显示前缀
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

	} ssLog_info_t;

private:
	FILE *m_f;
	bool m_create_logfile();

	std::string m_fileBaseName;
	std::string m_fileName;
	std::string m_logPath;
	std::string m_moduleName;
	bool m_isInit;
	bool m_sync;
	int m_flags;
	std::vector<std::string> m_level_map;
	unsigned int m_file_max_size;
	unsigned int m_file_sum;

#ifdef _WIN32
	std::mutex m_qMtx;
#else
	pthread_mutex_t *m_qMtx;
#endif
	std::queue<ssLogger::ssLog_info_t> *m_logQueue;

	std::string ssFormat(int length, const char *_format, va_list _vlist);

#ifdef _WIN32
	static void m_LogThread(void *_arg);
#else
	static void *m_LogThread(void *_arg);
#endif
	LOG_LEVEL m_logLevel;

#ifdef _WIN32
	void ss_lock(std::mutex &_mtx);
	void ss_unlock(std::mutex &_mtx);
#else
	void ss_lock(pthread_mutex_t &_mtx);
	void ss_unlock(pthread_mutex_t &_mtx);
#endif
};

#endif // !_SS_LOGGER_H_
