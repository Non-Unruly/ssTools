#include "ssLogger.h"

size_t ssLogger::size = 0;

#if defined _WIN32
std::mutex ssLogger::qMtx;
#else
pthread_mutex_t ssLogger::qMtx;
#endif

std::queue<ssLogger::ssLog_info_t> ssLogger::logQueue;
std::set<std::string> ssLogger::keys /*= { "%c","%d","%ld","%c","%s","%f","%lf" ,"%x" }*/;

std::string ssLogger::logNamePrefix;
size_t ssLogger::logFileSize = 1 * 1024 * 1024;

bool ssLogger::isInit = false;
ssLogger::LOG_LEVEL ssLogger::logLevel = ALL;

ssLogger::ssLogger()
{
}

bool ssLogger::init(const char *logPath, size_t fileSize, LOG_LEVEL level)
{
	std::vector<std::string> pathStep = ssTools::ss_split(logPath, "/");
	pathStep.erase((pathStep.end()--));

	std::string dir = ssTools::ss_keyJoint(pathStep, "/");
	if (strlen(logPath) > 0 && logPath[0] == '/')
		dir = "/" + dir;
	ssTools::ss_makePath(dir.c_str());

	isInit = false;
	logLevel = level;

	keys.insert("%c");
	keys.insert("%d");
	keys.insert("%ld");
	keys.insert("%c");
	keys.insert("%s");
	keys.insert("%f");
	keys.insert("%lf");
	keys.insert("%x");

	logNamePrefix = logPath;
	logFileSize = fileSize;

#if defined _WIN32
	std::thread thd(LogThread);
	isInit = true;
#else
	pthread_t thd_t;
	int res = pthread_create(&thd_t, NULL, &LogThread, NULL);
	if (res == 0)
		isInit = true;
#endif
	return isInit;
}

void ssLogger::output(bool print, int level, const char *functionName, int line, const char *format, ...)
{
	if (!isInit)
	{
		throw("ssLogger no initialize!!!");
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
	std::string log = "[" + ssTools::ss_datetime() + "]" + flag + function_line(functionName, line) + ssFormat(format, vlst);
	va_end(vlst);

	if (level >= logLevel)
	{
		ssLog_info_t t;
		t.log = log;
		t.isPrint = print;
		ss_lock(qMtx);
		logQueue.push(t);
		ss_unlock(qMtx);
	}

	return;
}

std::string ssLogger::ssFormat(const char *format, va_list vlist)
{
	std::string m(format);

	//计算全部关键词的信息
	std::vector<KEY_T> fmtInfos;
	for (int i = 0, len = m.length(); i < len; i++)
	{
		if (m[i] == '%')
		{
			std::string k2 = m.substr(i, 2);
			std::string k3 = m.substr(i, 3);
			if (keys.find(k2) != keys.end())
			{
				KEY_T t(k2, 2, i);
				fmtInfos.push_back(t);
			}
			else if (keys.find(k3) != keys.end())
			{
				KEY_T t(k3, 3, i);
				fmtInfos.push_back(t);
			}
		}
	}
	if (fmtInfos.size() == 0)
		return m;

	std::vector<std::string> elems;
	int last_pos = 0;
	for (std::vector<KEY_T>::iterator iter = fmtInfos.begin(); iter != fmtInfos.end(); iter++)
	{
		std::string elem;
		elem = m.substr(last_pos, (*iter).pos - last_pos);
		last_pos = (*iter).pos + (*iter).keylen;
		elems.push_back(elem);
	}
	std::string elem = m.substr(fmtInfos.back().pos + fmtInfos.back().keylen, -1);
	elems.push_back(elem);

	for (int i = 0, len = fmtInfos.size(); i < len; i++)
	{
		char *txt = NULL;
		if (fmtInfos[i].tagName == "%d")
		{
			txt = new char[32];
			int x = va_arg(vlist, int);
			sprintf(txt, "%d\0", x);
		}
		else if (fmtInfos[i].tagName == "%ld")
		{
			txt = new char[64];
			long x = va_arg(vlist, long);
			sprintf(txt, "%ld\0", x);
		}
		else if (fmtInfos[i].tagName == "%s")
		{
			std::string s = va_arg(vlist, const char *);
			txt = new char[s.length() + 1];
			sprintf(txt, "%s\0", s.c_str());
		}
		else if (fmtInfos[i].tagName == "%f")
		{
			txt = new char[64];
			float f = va_arg(vlist, float);
			sprintf(txt, "%f\0", f);
		}
		else if (fmtInfos[i].tagName == "%lf")
		{
			txt = new char[128];
			double f = va_arg(vlist, double);
			sprintf(txt, "%.8lf\0", f);
		}
		else if (fmtInfos[i].tagName == "%x")
		{
			txt = new char[32];
			int x = va_arg(vlist, int);
			sprintf(txt, "0x%02x\0", x);
		}
		else if (fmtInfos[i].tagName == "%c")
		{
			txt = new char[8];
			char c = va_arg(vlist, char);
			sprintf(txt, "%c\0", c);
		}
		fmtInfos[i].txt = std::string(txt);
		delete txt;
	}

	std::string log;
	std::vector<std::string>::iterator titer;
	std::vector<KEY_T>::iterator kiter = fmtInfos.begin();
	for (titer = elems.begin(); titer != elems.end(); titer++)
	{
		log += *titer;
		if (kiter != fmtInfos.end())
		{
			log += kiter->txt;
			kiter++;
		}
	}
	return log;
}

std::string ssLogger::function_line(const char *function, int line)
{
	std::string strfl = "";
	if (strlen(function) > 0 || line > 0)
	{
		char fl[512];
		sprintf(fl, "%s:%d | \0", function, line);
		strfl = fl;
	}
	return strfl;
}



#if defined _WIN32
void ssLogger::LogThread()
#else
void *ssLogger::LogThread(void *arg)
#endif
{
	std::string logTime = ssTools::ss_datetime();
	std::string fileName = logNamePrefix + ".log";
	FILE *f = fopen(fileName.c_str(), "at+");
	if (f == NULL)
	{
		throw("ssLogger create file error!");
		goto END;
	}
	size_t sum = 0;
	while (true)
	{
		while (true)
		{
			ss_lock(qMtx);
			int s = logQueue.size();
			ss_unlock(qMtx);
			if (s <= 0)
			{
#if defined _WIN32
				Sleep(50 * 1000);
#else
				usleep(50 * 1000);
#endif
			}
			else
			{
				break;
			}
		}
		ss_lock(qMtx);
		ssLog_info_t log_t = logQueue.front();
		logQueue.pop();
		ss_unlock(qMtx);
		std::string txt = log_t.log + "\n";
		if (log_t.isPrint == true)
			printf("%s", txt.c_str());
		fwrite(txt.c_str(), 1, txt.length(), f);
		fflush(f);
		sum += txt.length();

		if (sum >= logFileSize)
		{
			fclose(f);
			std::string newname = logNamePrefix + "-" + logTime + ".log";
			rename(fileName.c_str(), newname.c_str());
			f = NULL;
			sum = 0;
			logTime = ssTools::ss_datetime();
			fileName = logNamePrefix + ".log";
			f = fopen(fileName.c_str(), "at+");
			if (f == NULL)
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
void ssLogger::ss_lock(std::mutex &mtx)
{
	mtx.lock();
}
void ssLogger::ss_unlock(std::mutex &mtx)
{
	mtx.unlock();
}
#else
void ssLogger::ss_lock(pthread_mutex_t &mtx)
{
	pthread_mutex_lock(&mtx);
}
void ssLogger::ss_unlock(pthread_mutex_t &mtx)
{
	pthread_mutex_unlock(&mtx);
}
#endif
