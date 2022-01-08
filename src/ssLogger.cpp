//@ Project : ssTools
//@ Copyright : Non-unruly (Git)
//@ Git: https://github.com/Non-Unruly/ssTools
//@
//@ Launch Date : 2020-06-27
//@ Update: 2021-04-15

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

std::string ssLogger::m_logNamePrefix;
size_t ssLogger::m_logFileSize = 1 * 1024 * 1024;

bool ssLogger::m_isInit = false;
bool ssLogger::m_isTag[5] = { 0 };
bool ssLogger::m_sync = false;

ssLogger::LOG_LEVEL ssLogger::m_logLevel = ssLogger::LEVEL_ALL;

size_t ssLogger::m_maxLen = 2048;
FILE *ssLogger::m_f = NULL;
std::string ssLogger::m_fileName = "";

ssLogger::ssLogger()
{
}

bool ssLogger::init(const char *_logPath, size_t _fileSize, size_t _maxLen, LOG_LEVEL _level, LOG_TAG _tag, bool sync)
{
	std::vector<std::string> pathStep = ssTools::ss_split(_logPath, SEPARATOR);
	// pathStep.erase(--pathStep.end());

	std::string dir = ssTools::ss_keyJoint(pathStep, SEPARATOR);
	if (strlen(_logPath) > 0 && std::string(1, _logPath[0]) == SEPARATOR)
	{
		dir = SEPARATOR + dir;
	}
	ssTools::ss_makePath(dir.c_str());

	m_isInit = false;
	m_logLevel = _level;
	m_maxLen = _maxLen;
	m_sync = sync;


	int tag = _tag;
	for (int i = 0; i < 5; i++)
	{
		m_isTag[i] = tag & 1;
		tag = tag >> 1;
	}

	m_logNamePrefix = _logPath;
	m_logFileSize = _fileSize;

	m_fileName = m_logNamePrefix + ".log";
	m_f = fopen(m_fileName.c_str(), "at+");
	if (m_f == NULL)
	{
		printf("ssLogger create file error!\n");
		return false;
	}


	if (!m_sync)
	{
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
		m_isInit = true;
	}
	return m_isInit;
}

void ssLogger::output(bool print, int level, const char *srcName, const char *functionName, int line, const char *format, ...)
{
	if (!m_isInit)
	{
		//throw("ssLogger no initialize!!!");
		return;
	}
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
	va_list vlst;
	va_start(vlst, format);

	ssLog_info_t t(ssTools::ss_datetime(), flag, std::string(srcName), std::string(functionName), line, ssFormat(format, vlst), print);
	va_end(vlst);

	if (level >= m_logLevel)
	{
		ss_lock(m_qMtx);

		if (!m_sync)
		{
			m_logQueue.push(t);
		}
		else
		{
			std::string txt = show(t);
			fwrite(txt.c_str(), 1, txt.length(), m_f);
			fflush(m_f);
		}
		ss_unlock(m_qMtx);
	}

	return;
}

std::string ssLogger::ssFormat(const char *format, va_list vlist)
{
	char *str = new char[m_maxLen];
	memset(str, 0, m_maxLen);
	vsprintf(str, format, vlist);
	std::string res = std::string(str);
	delete[]str;
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

std::string ssLogger::show(ssLog_info_t info)
{
	std::string log;
	if (m_isTag[TAG_TIME])
		log += "[" + info.time + "]";
	if (m_isTag[TAG_LEVEL])
		log += info.type;
	if (m_isTag[TAG_FILE] || m_isTag[TAG_LINE])
	{
		char ll[16];
		sprintf(ll, "%d\0", info.line);
		log += "[" + info.file + ":" + std::string(ll) + "]";
	}
	if (m_isTag[TAG_FUNC])
		log += "[" + info.function + "]";

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
	size_t sum = 0;
	std::string logTime = ssTools::ss_datetime();
	while (true)
	{
		while (true)
		{
			ss_lock(m_qMtx);
			int s = m_logQueue.size();
			ss_unlock(m_qMtx);
			if (s <= 0)
			{
#if defined _WIN32
				Sleep(50);
#else
				usleep(50 * 1000);
#endif
			}
			else
			{
				break;
			}
		}
		ss_lock(m_qMtx);
		ssLog_info_t log_t = m_logQueue.front();
		m_logQueue.pop();
		ss_unlock(m_qMtx);
		std::string txt = show(log_t);
		fwrite(txt.c_str(), 1, txt.length(), m_f);
		fflush(m_f);
		sum += txt.length();

		if (sum >= m_logFileSize)
		{
			fclose(m_f);
			std::string newname = m_logNamePrefix + "-" + logTime + ".log";
			rename(m_fileName.c_str(), newname.c_str());
			m_f = NULL;
			sum = 0;
			logTime = ssTools::ss_datetime();
			m_fileName = m_logNamePrefix + ".log";
			m_f = fopen(m_fileName.c_str(), "at+");
			if (m_f == NULL)
			{
				throw("ssLogger create file error!");
				goto END;
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
