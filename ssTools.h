#ifndef _SSTOOLS_H_
#define _SSTOOLS_H_

#include <string>
#include <cstring>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <cstdlib>

#if defined _WIN32 || defined _WIN64
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#endif //  WIN

namespace ssTools
{
	//字符串分割 split string by key string
	//将str根据key进行分割，返回std::string顺序表
	std::vector<std::string> ss_split(std::string str, std::string key);

	//字符串拼接 string text joint by key string
	//使用key将字符顺序表elems进行连接，leftJoint/rightJoint确认字符串头左边或者右边是否需要添加key
	std::string ss_keyJoint(std::vector<std::string> elems, std::string key, bool leftJoint = false, bool rightJoint = false);

	//返回时间戳Linux
	unsigned int ss_timetick();

	//linux
	//格式： 年-月-日 时:分:秒:毫秒
	//format: "yyyy-MM-dd hh:mm:ss:ms
	std::string ss_datetime();

	//linux
	//递归创建目录 recursive make path
	bool ss_makePath(const char *path);
}


#endif

