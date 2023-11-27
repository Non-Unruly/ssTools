//@ Project : ssTools
//@ Copyright : Non-unruly (Git)
//@ Git: https://github.com/Non-Unruly/ssTools
//@
//@ Launch Date : 2020-06-27

#include "ssLogger.h"

#if defined _WIN32
std::mutex ssLogger::m_qMtx;
#define SEPARATOR "\\"
#else
pthread_mutex_t ssLogger::m_qMtx;
#define SEPARATOR "/"
#endif

std::queue<ssLogger::ssLog_info_t> ssLogger::m_logQueue;
std::set<std::string> ssLogger::m_keys;

bool ssLogger::m_isInit = false;
bool ssLogger::m_sync = false;

ssLogger::LOG_LEVEL ssLogger::m_logLevel = ssLogger::LEVEL_ALL;
FILE *ssLogger::m_f = NULL;
std::string ssLogger::m_fileBaseName = "";
std::string ssLogger::m_fileName = "";
std::string ssLogger::m_logPath = "./";
std::vector<std::string> ssLogger::m_level_map = std::vector<std::string>();
int ssLogger::m_flags = PREFIX_ALL;
unsigned int ssLogger::m_file_max_size = 256 * 1024 * 1024; // 256MB
unsigned int ssLogger::m_file_sum = 0;

ssLogger::ssLogger()
{
}

bool ssLogger::init(const char *_logPath, const char *_name, LOG_LEVEL _level, bool sync, unsigned int _logFileSize)
{
	m_fileBaseName = std::string(_name);
	m_logPath = _logPath;
	ssTools::ss_makePath(_logPath);
	m_file_max_size = _logFileSize < (1 * 1024 * 1024) ? (1024 * 1024) : _logFileSize;

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
	m_logLevel = _level;
	m_sync = sync;

	if (!m_create_logfile())
	{
		return false;
	}

	if (!m_sync)
	{
		// async mode 异步模式
#if defined _WIN32
		std::thread thd(m_LogThread);
		thd.detach();
		m_isInit = true;
#else
		pthread_t thd_t;
		int res = pthread_create(&thd_t, NULL, &m_LogThread, NULL);
		if (res == 0)
			m_isInit = true;
#endif
	}
	else
	{
		// sync mode 异步模式
		m_isInit = true;
	}
	return m_isInit;
}

std::string ssLogger::output(bool print, int level, const char *srcFileName, const char *functionName, int line, const char *format, ...)
{
	if (!m_isInit)
	{
		// throw("ssLogger no initialize!!!");
		return "";
	}

	ss_lock(m_qMtx);
	std::string prefix = "";
	std::string text;

	if ((m_flags & PREFIX_TIME) == PREFIX_TIME)
	{
		prefix += "[" + ssTools::ss_datetime() + "]";
	}
	if ((m_flags & PREFIX_LEVEL) == PREFIX_LEVEL)
	{
		prefix += m_level_map[level];
	}

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

	va_list args;
	va_start(args, format);
	int logLength = vsnprintf(nullptr, 0, format, args) + 1;
	va_end(args);

	va_list vlst;
	va_start(vlst, format);
	text = ssFormat(logLength, format, vlst);
	ssLog_info_t t(prefix, text, print);
	va_end(vlst);

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
					ss_unlock(m_qMtx);
					return text;
				}
			}
			char *log = new char[length];
			sprintf(log, "%s%s\n\0", prefix.c_str(), text.c_str());
			printf("%s", log);
			fwrite(log, 1, length, m_f);
//#ifdef _WIN32
//			fwrite("\n", 1, 1, m_f);
//#else
//			fwrite("\n", 1, 1, m_f);
//#endif
			m_file_sum += length;
			fflush(m_f);
			delete[] log;
		}
		else
		{
			// async mode
			m_logQueue.push(t);
		}
	}

	ss_unlock(m_qMtx);

	return text;
}

void ssLogger::SetLogPrefix(int flags)
{
	m_flags = flags;
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

#if defined _WIN32
void ssLogger::m_LogThread()
#else
void *ssLogger::m_LogThread(void *_arg)
#endif
{
	while (true)
	{
		while (m_isInit)
		{
			ss_lock(m_qMtx);
			int s = m_logQueue.size();
			if (s <= 0)
			{
				ss_unlock(m_qMtx);
#if defined _WIN32
				Sleep(50);
#else
				usleep(50 * 1000);
#endif
			}
			else
			{
				ssLog_info_t log_t = m_logQueue.front();
				m_logQueue.pop();
				size_t length = log_t.log.size();
				if (length + m_file_sum >= m_file_max_size)
				{
					m_isInit = m_create_logfile();
					if (!m_isInit)
					{
						ss_unlock(m_qMtx);
						continue;
					}
				}
				ss_unlock(m_qMtx);
				// if (!m_isInit)
				printf("%s", log_t.log.c_str());
				m_file_sum += length;
				fwrite(log_t.log.c_str(), 1, log_t.log.size(), m_f);
//#ifdef _WIN32
//				fwrite("\r\n", 1, 2, m_f);
//#else
//				fwrite("\n", 1, 1, m_f);
//#endif
				fflush(m_f);
				break;
			}
		}
	}
END:
#if defined _WIN32
	return;
#else
	return NULL;
#endif
}

#if defined _WIN32
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
