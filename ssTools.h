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
	//�ַ����ָ� split string by key string
	//��str����key���зָ����std::string˳���
	std::vector<std::string> ss_split(std::string str, std::string key);

	//�ַ���ƴ�� string text joint by key string
	//ʹ��key���ַ�˳���elems�������ӣ�leftJoint/rightJointȷ���ַ���ͷ��߻����ұ��Ƿ���Ҫ���key
	std::string ss_keyJoint(std::vector<std::string> elems, std::string key, bool leftJoint = false, bool rightJoint = false);

	//����ʱ���Linux
	unsigned int ss_timetick();

	//linux
	//��ʽ�� ��-��-�� ʱ:��:��:����
	//format: "yyyy-MM-dd hh:mm:ss:ms
	std::string ss_datetime();

	//linux
	//�ݹ鴴��Ŀ¼ recursive make path
	bool ss_makePath(const char *path);
}


#endif

