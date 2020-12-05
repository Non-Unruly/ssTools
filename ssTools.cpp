#include "ssTools.h"

std::vector<std::string> ssTools::ss_split(std::string str, std::string key)
{
	std::vector<std::string> texts;
	int keyLen = key.length();
	if (keyLen == 0)
	{
		texts.push_back(str);
		return texts;
	}

	std::string step;
	size_t pos = 0;
	while (pos < str.length())
	{
		int cur_pos = str.find(key, pos);
		step = str.substr(pos, cur_pos - pos);
		if (step.length() > 0)
			texts.push_back(step);
		step.clear();
		if (cur_pos == -1)
			break;
		pos = cur_pos + key.length();
	}

	return texts;
}

std::string ssTools::ss_keyJoint(std::vector<std::string> elems, std::string key, bool leftJoint, bool rightJoint)
{
	std::string str = "";
	if (leftJoint)
		str = key;
	for (std::vector<std::string>::iterator iter = elems.begin(); iter != elems.end(); iter++)
	{
		if (iter == (elems.end() - 1))
			str = str + (*iter);
		else
			str = str + (*iter) + key;
	}
	if (rightJoint)
		str = str + key;
	return str;
}

unsigned long ssTools::ss_timetick()
{
#if defined _WIN32 || defined _WIN64
	//windows 平台
	__int64 Freq = 0;
	__int64 Count = 0;
	if (QueryPerformanceFrequency((LARGE_INTEGER*)&Freq)
		&& Freq > 0
		&& QueryPerformanceCounter((LARGE_INTEGER*)&Count))
	{
		//乘以1000，把秒化为毫秒
		return (unsigned long)(double)Count / (double)Freq * 1000.0;
	}
	return 0.0;
#else
	//Linux 平台
	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_usec / 1000 + tv.tv_sec * 1000;
#endif // WIN32
}


std::string ssTools::ss_datetime()
{
	char result[1024];
	memset(result, 0, 1024);

#if defined _WIN32 || defined _WIN64
	//windows 平台
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	sprintf(result, "%04d%02d%02d %02d:%02d:%02d:%03ld",
		tm.wYear,
		tm.wMonth,
		tm.wDay,
		tm.wHour,
		tm.wMinute,
		tm.wSecond,
		tm.wMilliseconds
	);
	
#else
	//Linux 平台
	struct timeval tv;
	gettimeofday(&tv, 0);
	struct tm* pTempTm = localtime(&tv.tv_sec);
	if (pTempTm != NULL)
	{
		sprintf(result, "%04d%02d%02d %02d:%02d:%02d:%03ld",
			pTempTm->tm_year + 1900,
			pTempTm->tm_mon + 1,
			pTempTm->tm_mday,
			pTempTm->tm_hour,
			pTempTm->tm_min,
			pTempTm->tm_sec,
			tv.tv_usec / 1000);
	}
#endif // WIN32
	
	return std::string(result);
}

bool ssTools::ss_makePath(const char * path)
{
	if (strlen(path) <= 0)
		return false;

#if defined _WIN32 || defined _WIN64
	//windows 平台
	char *cmd = new char[32 + strlen(path)];
	sprintf(cmd, "md %s\0", cmd);
	system(cmd);
	delete[]cmd;

#else
	//Linux 平台
	std::vector<std::string> elems = ss_split(path, "/");
	if (path[0] == '/')
		elems[0] = '/' + elems[0];//如果从根目录开始，那么把第一个目录名前的'/'补上

	std::string fullPath;
	for (int i = 0; i < elems.size(); i++)
	{
		fullPath = fullPath + elems[i] + "/";
		int res = mkdir(fullPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
		if (res != 0)
		{
			if (errno == 17)
				continue;//17错误，当前目录已存在，继续创建下一层
			std::cout << "mkdir error: " << errno << " " << fullPath << std::endl;
			return false;
		}
	}
#endif
	return true;
}

short ssTools::hex2i(const char * _hex)
{
	short res = 0;
	std::string hex(_hex);
	if (hex.length() != 2)
		return -1;

	std::transform(hex.begin(), hex.end(), hex.begin(), toupper);
	for (int i = 0; i < 2; i++)
	{
		if (hex[i] >= '0' && hex[i] <= '9')
		{
			res += (hex[i] - '0')*pow(16, 1 - i);
			continue;
		}
		else if (hex[i] >= 'A' && hex[i] <= 'F')
		{
			res += (hex[i] - 'A' + 10)*pow(16, 1 - i);;
			continue;
		}
		else
		{
			return -1;
		}
	}
	return res;
}
