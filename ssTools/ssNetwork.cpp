#include "ssNetwork.h"

namespace ssTools
{

	std::string GetLocalIPv4(std::vector<std::string> &ip_lst)
	{
		std::string ip;
#ifdef _WIN32
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			printf("WSAStartup failed with error: %d\n", iResult);
			return "";
		}
#endif
		//获取本地计算机主机名


		char	strName[1024];
		gethostname(strName, 1024);

		//获取本地计算机信息
		struct hostent *pHostEnt = gethostbyname(strName);
		if (pHostEnt == NULL)
		{
			return "";
		}

		//获取本地计算机IP地址
		int n = 0;
		while (pHostEnt->h_addr_list[n] != NULL)
		{
			char tt[1024];
			sprintf(tt, "%d.%d.%d.%d\0", pHostEnt->h_addr_list[n][0] & 0x00FF, pHostEnt->h_addr_list[n][1] & 0x00FF, pHostEnt->h_addr_list[n][2] & 0x00FF, pHostEnt->h_addr_list[n][3] & 0x00FF);
			ip_lst.push_back(std::string(tt));
			n++;
		}

		WSACleanup();
		
		ip = ss_keyJoint(ip_lst, "|");

		return ip;
	}
}
//
//#include <iostream>
//#include <string>
//#include <Winsock2.h>
//#include <sstream>
//
//using namespace std;
//
//#pragma comment(lib, "ws2_32.lib")
//
//string	GetLocalIP()
//{
//	string strIP("127.0.0.1");
//
//	//初始化sock
//	WSADATA WSAData;
//	if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
//	{
//		return strIP;
//	}
//
//	//获取本地计算机主机名
//	char	strName[1024];
//	gethostname(strName, 1024);
//
//	//获取本地计算机信息
//	struct hostent *pHostEnt = gethostbyname(strName);
//	if (pHostEnt == NULL)
//	{
//		return strIP;
//	}
//
//	//获取本地计算机IP地址
//	int n = 0;
//	while (pHostEnt->h_addr_list[n] != NULL)
//	{
//		stringstream ss;
//		ss << (pHostEnt->h_addr_list[n][0] & 0x00FF) << "."
//			<< (pHostEnt->h_addr_list[n][1] & 0x00FF) << "."
//			<< (pHostEnt->h_addr_list[n][2] & 0x00FF) << "."
//			<< (pHostEnt->h_addr_list[n][3] & 0x00FF) << ends;
//
//		string strTemp(ss.str());
//
//
//		if (string::npos != strTemp.find("192.168.1."))
//		{
//			strIP = strTemp;
//			break;
//		}
//		else
//		{
//			n++;
//		}
//	}
//
//	WSACleanup();
//	return strIP;
//}
//
//int main()
//{
//	cout << GetLocalIP() << endl;
//
//	return 0;
//}