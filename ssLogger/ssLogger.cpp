#include "ssLogger.h"

size_t ssLogger::size = 0;
std::vector<ssLog_info_t> ssLogger::logArray;
std::set<std::string> ssLogger::keys = { "%c","%d","%ld","%c","%s","%f","%lf" ,"%x" };


bool ssLogger::init(const char * logPath, size_t fileSize)
{
	return false;
}

void ssLogger::debug(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string log = ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = false;
	logArray.push_back(t);
}

void ssLogger::info(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string log = ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = false;
	logArray.push_back(t);
}

void ssLogger::warning(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string log = ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = false;
	logArray.push_back(t);
}

void ssLogger::error(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string log = ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = false;
	logArray.push_back(t);
}

void ssLogger::disaster(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string log = ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = false;
	logArray.push_back(t);
}

void ssLogger::debugp(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string log = ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = true;
	logArray.push_back(t);
}

void ssLogger::infop(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string log = ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = true;
	logArray.push_back(t);
}

void ssLogger::warningp(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string log = ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = true;
	logArray.push_back(t);
}

void ssLogger::errorp(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string log = ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = true;
	logArray.push_back(t);
}

void ssLogger::disasterp(const char * format, ...)
{
	va_list vlst;
	va_start(vlst, format);
	std::string log = ssFormat(format, vlst);
	va_end(vlst);

	ssLog_info_t t;
	t.log = log;
	t.isPrint = true;
	logArray.push_back(t);
}

std::string ssLogger::ssFormat(const char * format, va_list vaList)
{
	std::string m(format);
	//std::cout << m << std::endl;

	//计算全部关键词的信息
	std::vector<key_t> fmtInfos;
	for (int i = 0, len = m.length(); i < len; i++)
	{
		if (m[i] == '%')
		{
			std::string k2 = m.substr(i, 2);
			std::string k3 = m.substr(i, 3);
			if (keys.find(k2) != keys.end())
			{
				key_t t(k2, 2, i);
				fmtInfos.push_back(t);
			}
			else if (keys.find(k3) != keys.end())
			{
				key_t t(k3, 3, i);
				fmtInfos.push_back(t);
			}
		}
	}

	std::vector<std::string> elems;
	int last_pos = 0;
	for (std::vector<key_t>::iterator iter = fmtInfos.begin(); iter != fmtInfos.end(); iter++)
	{
		std::string elem;
		elem = m.substr(last_pos, (*iter).pos - last_pos);
		last_pos = (*iter).pos + (*iter).keylen;
		elems.push_back(elem);
	}
	std::string elem = m.substr(fmtInfos.back().pos + fmtInfos.back().keylen, -1);
	elems.push_back(elem);


	va_list arglist = vaList;
	for (int i = 0, len = fmtInfos.size(); i < len; i++)
	{
		char txt[128];
		if (fmtInfos[i].tagName == "%d")
		{
			int x = va_arg(arglist, int);
			sprintf(txt, "%d\0", x);
		}
		else if (fmtInfos[i].tagName == "%ld")
		{
			long x = va_arg(arglist, long);
			sprintf(txt, "%ld\0", x);
		}
		else if (fmtInfos[i].tagName == "%s")
		{
			std::string s = va_arg(arglist, const char*);
			sprintf(txt, "%s\0", s.c_str());
		}
		else if (fmtInfos[i].tagName == "%f")
		{
			float f = va_arg(arglist, float);
			sprintf(txt, "%f\0", f);
		}
		else if (fmtInfos[i].tagName == "%lf")
		{
			double f = va_arg(arglist, double);
			sprintf(txt, "%.8lf\0", f);
		}
		else if (fmtInfos[i].tagName == "%x")
		{
			int x = va_arg(arglist, int);
			sprintf(txt, "0x%02x\0", x);
		}
		else if (fmtInfos[i].tagName == "%c")
		{
			char c = va_arg(arglist, char);
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
	std::vector<key_t>::iterator kiter = fmtInfos.begin();
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

void ssLogger::insert(ssLog_info_t t)
{
}

void ssLogger::out()
{
	for (std::vector<ssLog_info_t>::iterator iter = logArray.begin(); iter != logArray.end(); iter++)
	{
		std::cout << iter->log << std::endl;
	}
	logArray.clear();
}
