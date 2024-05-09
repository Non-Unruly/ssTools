//@ Project : ssTools
//@ Copyright : Non-unruly (Git)
//@ Git: https://github.com/Non-Unruly/ssTools
//@
//@ Launch Date : 2020-06-27

#include "ssLogger.h"

#ifdef _WIN32
#define SEPARATOR "\\"
#else
#define SEPARATOR "/"
#endif

std::vector<ssLogger *> ssLogger::m_loggers;

int ssLogger::Register(const char *_logPath, const char *_logFileName, const char *_logModuelName, LOG_LEVEL _level, bool _sync, unsigned int _logFileSize)
{
	for (int i = 0; i < m_loggers.size(); i++)
	{
		if (m_loggers[i]->GetModName() == std::string(_logModuelName))
			return i;
	}

	std::string mod(_logModuelName);
	ssLogger *logPtr = new ssLogger(_logPath, _logFileName, _logModuelName, _level, _sync, _logFileSize);
	m_loggers.push_back(logPtr);
	return m_loggers.size() - 1;
}

ssLogger ssLogger::Logger(const char *module)
{
	for (int i = 0; i < m_loggers.size(); i++)
	{
		if (m_loggers[i]->GetModName() == std::string(module))
		{
			return (*m_loggers[i]);
		}
	}
	int index = ssLogger::Register("./log", module, module);
	return (*m_loggers[index]);
}

ssLogger::ssLogger(const char *_logPath, const char *_logFileName, const char *_logModuleName, LOG_LEVEL _level, bool _sync, unsigned int _logFileSize)
	: m_f(0), m_qMtx(0), m_logQueue(NULL)
{
	init(_logPath, _logFileName, _logModuleName, _level, _sync, _logFileSize);
}

bool ssLogger::init(const char *_logPath, const char *_logFileName, const char *_logModuleName, LOG_LEVEL _level, bool sync, unsigned int _logFileSize)
{
#if defined(_WIN32) || __cplusplus >= 201103L
	m_qMtx = new std::mutex;
#else
	m_qMtx = new pthread_mutex_t();
#endif
	m_fileBaseName = std::string(_logFileName);
	if (m_fileBaseName.length() == 0)
		m_fileBaseName = "LOG";
	m_logPath = _logPath;
	m_moduleName = _logModuleName;
	ssTools::ss_makePath(_logPath);
	m_file_max_size = _logFileSize < (1 * 1024 * 1024) ? (1024 * 1024) : _logFileSize;
	m_logLevel = _level;
	m_sync = sync;
	m_flags = PREFIX_ALL;

	m_level_map.reserve(6);
	m_level_map.resize(6);
	m_level_map[LEVEL_ALL] = "[ALL]"; // 占位
	m_level_map[LEVEL_DBG] = "[DBG]";
	m_level_map[LEVEL_INF] = "[INF]";
	m_level_map[LEVEL_WAR] = "[WAR]";
	m_level_map[LEVEL_ERR] = "[ERR]";
	m_level_map[LEVEL_DISASTER] = "[DIS]";

	m_isInit = false;

	if (!m_create_logfile())
	{
		return false;
	}

	if (!m_sync)
	{
		m_logQueue = new std::queue<ssLogger::ssLog_info_t>();
		// async mode 异步模式
#if defined(_WIN32) || __cplusplus >= 201103L
		std::thread thd(m_LogThread,this);
		thd.detach();
		m_isInit = true;
#else
		// 为了保证兼容性，linux采用系统api来实现多线程
		pthread_t thd_t;
		printf("sync thread ptr : %p\n", this);
		int res = pthread_create(&thd_t, NULL, &m_LogThread, this);
		if (res == 0)
			m_isInit = true;
		else
			printf("create log thread failed\n");
#endif
	}
	else
	{
		// sync mode 同步模式
		m_isInit = true;
	}
	return m_isInit;
}

std::string ssLogger::Debug(bool print, const char *srcName, const char *funcName, int line, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	int logLength = vsnprintf(nullptr, 0, format, args) + 1;
	va_end(args);

	va_list vlst;
	va_start(vlst, format);
	std::string text = ssFormat(logLength, format, vlst);
	va_end(vlst);

	std::string str = output(print, LEVEL_DBG, srcName, funcName, line, text);

	return str;
}

std::string ssLogger::Info(bool print, const char *srcName, const char *funcName, int line, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	int logLength = vsnprintf(nullptr, 0, format, args) + 1;
	va_end(args);

	va_list vlst;
	va_start(vlst, format);
	std::string text = ssFormat(logLength, format, vlst);
	va_end(vlst);

	std::string str = output(print, LEVEL_INF, srcName, funcName, line, text);

	return str;
}

std::string ssLogger::Warn(bool print, const char *srcName, const char *funcName, int line, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	int logLength = vsnprintf(nullptr, 0, format, args) + 1;
	va_end(args);

	va_list vlst;
	va_start(vlst, format);
	std::string text = ssFormat(logLength, format, vlst);
	va_end(vlst);

	std::string str = output(print, LEVEL_WAR, srcName, funcName, line, text);

	return str;
}

std::string ssLogger::Error(bool print, const char *srcName, const char *funcName, int line, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	int logLength = vsnprintf(nullptr, 0, format, args) + 1;
	va_end(args);

	va_list vlst;
	va_start(vlst, format);
	std::string text = ssFormat(logLength, format, vlst);
	va_end(vlst);

	std::string str = output(print, LEVEL_ERR, srcName, funcName, line, text);

	return str;
}

std::string ssLogger::Disaster(bool print, const char *srcName, const char *funcName, int line, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	int logLength = vsnprintf(nullptr, 0, format, args) + 1;
	va_end(args);

	va_list vlst;
	va_start(vlst, format);
	std::string text = ssFormat(logLength, format, vlst);
	va_end(vlst);

	std::string str = output(print, LEVEL_DISASTER, srcName, funcName, line, text);

	return str;
}

std::string ssLogger::output(bool print, int level, const char *srcFileName, const char *functionName, int line, std::string text)
{
	if (!m_isInit)
	{
		// throw("ssLogger no initialize!!!");
		return "";
	}

	ss_lock(*m_qMtx);
	std::string prefix = "";

	if ((m_flags & PREFIX_TIME) == PREFIX_TIME)
	{
		prefix += "[" + ssTools::ss_datetime() + "]";
	}
	if ((m_flags & PREFIX_LEVEL) == PREFIX_LEVEL)
	{
		prefix += m_level_map[level];
	}
	prefix += "[" + (m_moduleName == "" ? "MAIN" : m_moduleName) + "]";

	std::string preFile;
	std::string preFunc;
	std::string preLine;
	std::vector<std::string> srcInfoVec;
	if ((m_flags & PREFIX_FILE) == PREFIX_FILE)
	{
		std::vector<std::string> p = ssTools::ss_split(srcFileName, SEPARATOR);
		srcInfoVec.push_back(p.back());
	}
	if ((m_flags & PREFIX_FUNC) == PREFIX_FUNC)
	{
		srcInfoVec.push_back(std::string(functionName));
	}
	if ((m_flags & PREFIX_LINE) == PREFIX_LINE)
	{
		char lineStr[16];
		sprintf(lineStr, "%d\0", line);
		srcInfoVec.push_back(std::string(lineStr));
	}

	std::string srcInfo = ssTools::ss_keyJoint(srcInfoVec, ":");
	if (srcInfo.size() > 0)
		srcInfo = "[" + srcInfo + "]";
	prefix += srcInfo;

	ssLog_info_t t(prefix, text, print);

	if (level >= m_logLevel)
	{
		if (m_sync)
		{
			// sync mode
			size_t length = prefix.size() + text.size() + 2;
			if (m_file_sum + length >= m_file_max_size)
			{
				m_isInit = m_create_logfile();
				if (!m_isInit)
				{
					ss_unlock(*m_qMtx);
					return text;
				}
			}
			char *log = new char[length];
			memset(log, 0, length);
			sprintf(log, "%s%s\n\0", prefix.c_str(), text.c_str());
			printf("%s", log);
			fwrite(log, 1, strlen(log), m_f);
			// #ifdef _WIN32
			//			fwrite("\n", 1, 1, m_f);
			// #else
			//			fwrite("\n", 1, 1, m_f);
			// #endif
			m_file_sum += length;
			fflush(m_f);
			delete[] log;
		}
		else
		{
			// async mode
			m_logQueue->push(t);
		}
	}

	ss_unlock(*m_qMtx);

	return text;
}

std::string ssLogger::GetModName()
{
	return m_moduleName;
}

bool ssLogger::m_create_logfile()
{
	if (m_f != NULL)
	{
		fflush(m_f);
		fclose(m_f);
	}
	m_file_sum = 0;
	m_fileName = m_logPath + "/" + m_fileBaseName + "-" + ssTools::ss_datetime_simple() + ".log";
	// printf("ssTools create log file: %s\n", m_fileName.c_str());
	m_f = fopen(m_fileName.c_str(), "at+");
	if (m_f == NULL)
	{
		printf("ssLogger create file error!\n");
		return false;
	}
	return true;
}

std::string ssLogger::ssFormat(int length, const char *format, va_list vlist)
{
	char *str = new char[length];
	memset(str, 0, length);
	vsprintf(str, format, vlist);
	std::string res = std::string(str);
	delete[] str;
	return res;
}

#if defined(_WIN32) || __cplusplus >= 201103L
void ssLogger::m_LogThread(void *_arg)
#else
void *ssLogger::m_LogThread(void *_arg)
#endif
{
	ssLogger *self = (ssLogger *)_arg;
	while (true)
	{
		while (self->m_isInit)
		{
			self->ss_lock(*self->m_qMtx);
			int s = self->m_logQueue->size();
			if (s <= 0)
			{
				self->ss_unlock(*self->m_qMtx);
#ifdef _WIN32
				Sleep(50);
#else
				usleep(50 * 1000);
#endif
			}
			else
			{
				ssLog_info_t log_t = self->m_logQueue->front();
				self->m_logQueue->pop();
				size_t length = log_t.log.size();
				if (length + self->m_file_sum >= self->m_file_max_size)
				{
					self->m_isInit = self->m_create_logfile();
					if (!self->m_isInit)
					{
						self->ss_unlock(*self->m_qMtx);
						continue;
					}
				}
				self->ss_unlock(*self->m_qMtx);
				// if (!m_isInit)
				printf("%s", log_t.log.c_str());
				self->m_file_sum += length;
				fwrite(log_t.log.c_str(), 1, log_t.log.size(), self->m_f);
				// #ifdef _WIN32
				//				fwrite("\r\n", 1, 2, m_f);
				// #else
				//				fwrite("\n", 1, 1, m_f);
				// #endif
				fflush(self->m_f);
				break;
			}
		}
	}
END:
#if defined(_WIN32) || __cplusplus >= 201103L
	return;
#else
	return NULL;
#endif
}

#if defined(_WIN32) || __cplusplus >= 201103L
void ssLogger::ss_lock(std::mutex &_mtx)
{
	_mtx.lock();
}
void ssLogger::ss_unlock(std::mutex &_mtx)
{
	_mtx.unlock();
}
#else
void ssLogger::ss_lock(pthread_mutex_t &_mtx)
{
	pthread_mutex_lock(&_mtx);
}
void ssLogger::ss_unlock(pthread_mutex_t &_mtx)
{
	pthread_mutex_unlock(&_mtx);
}
#endif
