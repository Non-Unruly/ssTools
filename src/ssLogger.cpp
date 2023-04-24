//@ Project : ssTools
//@ Copyright : Non-unruly (Git)
//@ Git: https://github.com/Non-Unruly/ssTools
//@
//@ Launch Date : 2020-06-27

#include "ssLogger.h"

size_t ssLogger::m_size = 0;

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
std::string ssLogger::m_fileName = "";

ssLogger::ssLogger()
{
}

bool ssLogger::init(const char *_logPath, const char *_name, LOG_LEVEL _level, bool sync)
{
	// try to create log's dir
	ssTools::ss_makePath(_logPath);

	m_isInit = false;
	m_logLevel = _level;
	m_sync = sync;

	m_fileName = std::string(_logPath) + "/" + std::string(_name) + "-" + ssTools::ss_datetime_simple("-") + ".log";
	printf("ssTools create log file: %s\n", m_fileName.c_str());
	m_f = fopen(m_fileName.c_str(), "at+");
	if (m_f == NULL)
	{
		printf("ssLogger create file error!\n");
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

void ssLogger::output(bool print, int level, const char *srcName, const char *functionName, int line, const char *format, ...)
{
	if (!m_isInit)
	{
		// throw("ssLogger no initialize!!!");
		return;
	}
	
	ss_lock(m_qMtx);

	std::string flag;
	switch (level)
	{
	case 1:
		flag = "[DBG]";
		break;
	case 2:
		flag = "[INF]";
		break;
	case 3:
		flag = "[WAR]";
		break;
	case 4:
		flag = "[ERR]";
		break;
	case 5:
		flag = "[DIS]";
		break;
	default:
		flag = "[DBG]";
		break;
	}

	va_list args;
	va_start(args, format);
	int logLength = vsnprintf(nullptr, 0, format, args) + 1;
	va_end(args);

	va_list vlst;
	va_start(vlst, format);
	ssLog_info_t t(ssTools::ss_datetime(), flag, std::string(srcName), std::string(functionName), line, ssFormat(logLength, format, vlst), print);
	va_end(vlst);

	if (level >= m_logLevel)
	{
		if (!m_sync)
		{
			// async mode
			m_logQueue.push(t);
		}
		else
		{
			// sync mode
			std::string txt = show_text(t);
			fwrite(txt.c_str(), 1, txt.length(), m_f);
			fflush(m_f);
		}
	}

	ss_unlock(m_qMtx);

	return;
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

std::string ssLogger::function_line(const char *src, const char *function, int line)
{
	std::string strfl = "";
	if (strlen(function) > 0 || line > 0)
	{
		char fl[512];
		sprintf(fl, "[%s][%s:%d]\0", src, function, line);
		strfl = fl;
	}
	return strfl;
}

std::string ssLogger::show_text(ssLog_info_t info)
{
	std::string log;

	log += "[" + info.time + "]";
	log += info.type;

	char ll[16];
	sprintf(ll, "%d\0", info.line);
	log += "[" + info.file + " | " + info.function + ":" + std::string(ll) + "] ";

	log += info.log;
	log += "\n";

	if (info.isPrint)
	{
		printf("%s", log.c_str());
	}
	return log;
}

#if defined _WIN32
void ssLogger::m_LogThread()
#else
void *ssLogger::m_LogThread(void *_arg)
#endif
{
	std::string logTime = ssTools::ss_datetime();
	while (true)
	{
		while (true)
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
				ss_unlock(m_qMtx);
				std::string txt = show_text(log_t);
				fwrite(txt.c_str(), 1, txt.length(), m_f);
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
