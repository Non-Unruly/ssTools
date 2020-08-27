#include "ssLogger.h"

size_t ssLogger::size = 0;
std::queue<ssLog_info_t> ssLogger::logQueue;
std::set<std::string> ssLogger::keys = { "%c","%d","%ld","%c","%s","%f","%lf" ,"%x" };

std::string ssLogger::logNamePrefix = "ssLog";
size_t ssLogger::logFileSize = 1 * 1024 * 1024;

unsigned int timetick()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_usec / 1000 + tv.tv_sec * 1000;
}

std::string DateTime()
{
	static char timestr[200] = { 0 };
	struct tm *pTempTm;
	struct timeval time;

	gettimeofday(&time, NULL);
	pTempTm = localtime(&time.tv_sec);
	if (NULL != pTempTm)
	{
		snprintf(timestr, 199, "[%04d-%02d-%02d %02d:%02d:%02d:%03ld]",
			pTempTm->tm_year + 1900,
			pTempTm->tm_mon + 1,
			pTempTm->tm_mday,
			pTempTm->tm_hour,
			pTempTm->tm_min,
			pTempTm->tm_sec,
			time.tv_usec / 1000);
	}
	return std::string(timestr);
}

std::string Time()
{
	static char timestr[200] = { 0 };
	struct tm *pTempTm;
	struct timeval time;

	gettimeofday(&time, NULL);
	pTempTm = localtime(&time.tv_sec);
	if (NULL != pTempTm)
	{
		snprintf(timestr, 199, "%02d%02d%02d-%02d%02d%02d",
			(pTempTm->tm_year + 1900) % 100,
			pTempTm->tm_mon + 1,
			pTempTm->tm_mday,
			pTempTm->tm_hour,
			pTempTm->tm_min,
			pTempTm->tm_sec);
	}
	return std::string(timestr);
}


bool ssLogger::init(const char * logPath, size_t fileSize)
{
	logNamePrefix = logPath;
	logFileSize = fileSize;
	
	pthread_t thd_t;
	int res = pthread_create(&thd_t, NULL, &LogThread, NULL);
	std::cout << "pthread create res= " << res << std::endl;
	return true;
}

void ssLogger::debug(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string prefix = DateTime() + "[DBG]";
	std::string log = prefix + ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = false;
	logQueue.push(t);
}

void ssLogger::info(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string prefix = DateTime()+ "[INF]";
	std::string log = ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = false;
	logQueue.push(t);
}

void ssLogger::warning(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string prefix = DateTime() + "[WAR]";
	std::string log = prefix + ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = false;
	logQueue.push(t);
}

void ssLogger::error(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string prefix = DateTime() + "[ERR]";
	std::string log = prefix + ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = false;
	logQueue.push(t);
}

void ssLogger::disaster(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string prefix = DateTime() + "[DIS]";
	std::string log = prefix + ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = false;
	logQueue.push(t);
}

void ssLogger::debugp(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string prefix = DateTime() + "[DBG]";
	std::string log = prefix + ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = true;
	logQueue.push(t);
}

void ssLogger::infop(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string prefix = DateTime() + "[INF]";
	std::string log = prefix + ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = true;
	logQueue.push(t);
}

void ssLogger::warningp(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string prefix = DateTime() + "[WAR]";
	std::string log = prefix + ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = true;
	logQueue.push(t);
}

void ssLogger::errorp(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string prefix = DateTime() + "[ERR]";
	std::string log = prefix + ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = true;
	logQueue.push(t);
}

void ssLogger::disasterp(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string prefix = DateTime() + "[DIS]";
	std::string log = prefix + ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = true;
	logQueue.push(t);
}

std::string ssLogger::ssFormat(const char * format, va_list vlist)
{
	std::string m(format);
	//std::cout << m << std::endl;

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


	//va_list arglist = vlist;
	//memcpy(&arglist, &vaList, sizeof(vaList));
	for (int i = 0, len = fmtInfos.size(); i < len; i++)
	{
		char txt[128];
		if (fmtInfos[i].tagName == "%d")
		{
			int x = va_arg(vlist, int);
			sprintf(txt, "%d\0", x);
		}
		else if (fmtInfos[i].tagName == "%ld")
		{
			long x = va_arg(vlist, long);
			sprintf(txt, "%ld\0", x);
		}
		else if (fmtInfos[i].tagName == "%s")
		{
			std::string s = va_arg(vlist, const char*);
			sprintf(txt, "%s\0", s.c_str());
		}
		else if (fmtInfos[i].tagName == "%f")
		{
			float f = va_arg(vlist, float);
			sprintf(txt, "%f\0", f);
		}
		else if (fmtInfos[i].tagName == "%lf")
		{
			double f = va_arg(vlist, double);
			sprintf(txt, "%.8lf\0", f);
		}
		else if (fmtInfos[i].tagName == "%x")
		{
			int x = va_arg(vlist, int);
			sprintf(txt, "0x%02x\0", x);
		}
		else if (fmtInfos[i].tagName == "%c")
		{
			char c = va_arg(vlist, char);
			sprintf(txt, "%c\0", c);
		}
		fmtInfos[i].txt = std::string(txt);
	}

	//for (auto x : elems)
	//	std::cout << x << " $ ";
	//std::cout << std::endl;
	//for (auto x : fmtInfos)
	//	std::cout << x.txt << " # ";
	//std::cout << std::endl;

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

//void ssLogger::insert(ssLog_info_t t)
//{
//}

void * ssLogger::LogThread(void * arg)
{
	std::cout << "thread in" << std::endl;
	std::string fileName = logNamePrefix + "-" + Time() + ".ssLog";
	FILE *f = fopen(fileName.c_str(), "at+");
	if (f == NULL)
	{
		throw("ssLogger create file error!");
		return NULL;
	}
	size_t sum = 0;
	while (true)
	{
		while (logQueue.size() <= 0)
		{
			usleep(5);
		}
		ssLog_info_t log_t = logQueue.front();
		logQueue.pop();
		std::string txt = log_t.log + "\n";
		//std::cout << txt << std::endl;
		if (log_t.isPrint == true)
			printf("%s", txt.c_str());
		fwrite(txt.c_str(), 1, txt.length(), f);
		fflush(f);
		sum += txt.length();
		
		if (sum >= logFileSize)
		{
			std::cout << "size out" << std::endl;
			fclose(f);
			f = NULL;
			sum = 0;
			fileName = logNamePrefix + "-" + Time() + ".ssLog";
			f = fopen(fileName.c_str(), "at+");
			if (f == NULL)
			{
				throw("ssLogger create file error!");
				return NULL;
			}
		}
	}
	return NULL;
}

//void ssLogger::out()
//{
//	std::stack<ssLog_info_t> stk = logStack;
//	while (stk.size() == 0)
//	{
//		std::string log = stk.top().log;
//		std::cout << log << std::endl;
//		stk.pop();
//	}
//}
