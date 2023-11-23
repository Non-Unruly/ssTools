#include "ssNetwork.h"

namespace ssTools
{

	std::vector<std::string> GetLocalIPv4(std::string &addrs)
	{
		std::string ip;
		std::vector<std::string> ip_lst;
#ifdef _WIN32
		// Windows
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0)
		{
			printf("WSAStartup failed with error: %d\n", iResult);
			return "";
		}

		char strName[1024];
		gethostname(strName, 1024);

		// ��ȡ���ؼ�������?
		struct hostent *pHostEnt = gethostbyname(strName);
		if (pHostEnt == NULL)
		{
			return "";
		}

		// ��ȡ���ؼ����IP��ַ
		int n = 0;
		while (pHostEnt->h_addr_list[n] != NULL)
		{
			char tt[1024];
			sprintf(tt, "%d.%d.%d.%d\0", pHostEnt->h_addr_list[n][0] & 0x00FF, pHostEnt->h_addr_list[n][1] & 0x00FF, pHostEnt->h_addr_list[n][2] & 0x00FF, pHostEnt->h_addr_list[n][3] & 0x00FF);
			ip_lst.push_back(std::string(tt));
			n++;
		}

		WSACleanup();

		addrs = ss_keyJoint(ip_lst, ",");
#else
		// Linux
		struct ifaddrs *ifAddrStruct = NULL;
		void *tmpAddrPtr = NULL;

		getifaddrs(&ifAddrStruct);

		while (ifAddrStruct != NULL)
		{
			if (ifAddrStruct->ifa_addr != NULL)
			{
				if (ifAddrStruct->ifa_addr->sa_family == AF_INET)
				{
					tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
					char addressBuffer[INET_ADDRSTRLEN];
					inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
					std::string ipstr(addressBuffer);
					if (ipstr != "127.0.0.1")
						ip_lst.push_back(ipstr);
				}
			}
			// else if (ifAddrStruct->ifa_addr->sa_family == AF_INET6)
			// { // check it is IP6
			//     // is a valid IP6 Address
			//     tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
			//     char addressBuffer[INET6_ADDRSTRLEN];
			//     inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
			//     printf("%s IP Address %s\n", ifAddrStruct->ifa_name, addressBuffer);
			// }
			ifAddrStruct = ifAddrStruct->ifa_next;
		}
		for (int i = 0; i < ip_lst.size(); i++)
		{
			addrs += ip_lst[i];
			if (i < ip_lst.size() - 1)
				addrs += ",";
		}

#endif

		return ip_lst;
	}
}
//
// #include <iostream>
// #include <string>
// #include <Winsock2.h>
// #include <sstream>
//
// using namespace std;
//
// #pragma comment(lib, "ws2_32.lib")
//
// string	GetLocalIP()
//{
//	string strIP("127.0.0.1");
//
//	//��ʼ��sock
//	WSADATA WSAData;
//	if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
//	{
//		return strIP;
//	}
//
//	//��ȡ���ؼ����������?
//	char	strName[1024];
//	gethostname(strName, 1024);
//
//	//��ȡ���ؼ�������?
//	struct hostent *pHostEnt = gethostbyname(strName);
//	if (pHostEnt == NULL)
//	{
//		return strIP;
//	}
//
//	//��ȡ���ؼ����IP��ַ
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
// int main()
//{
//	cout << GetLocalIP() << endl;
//
//	return 0;
//}