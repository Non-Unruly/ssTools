#include "ssNetwork.h"

namespace ssTools
{

	std::vector<std::string> GetLocalIPv4(std::string &addrs, const char *separator)
	{
		std::vector<std::string> ip_lst;
#ifdef _WIN32
		// Windows
		WSADATA wsaData;
		int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (res != 0)
		{
			std::cout << "winsock initialize failed" << std::endl;
			return ip_lst;
		}

		char hostName[128];
		int len = gethostname(hostName, 128);

		struct addrinfo nif;
		memset(&nif, 0, sizeof(nif));
		nif.ai_family = AF_UNSPEC;
		nif.ai_socktype = SOCK_STREAM;
		nif.ai_protocol = IPPROTO_TCP;

		struct addrinfo *addrInfos = nullptr;
		getaddrinfo(hostName, "", &nif, &addrInfos);
		while (true)
		{
			if (addrInfos == nullptr)
				break;
			struct sockaddr_in *addr = (struct sockaddr_in *)addrInfos->ai_addr;
			char *ip = inet_ntoa(addr->sin_addr);
			std::string ipstr = std::string(ip);
			if (ipstr != "0.0.0.0" && ipstr != "127.0.0.1")
				ip_lst.push_back(std::string(ip));
			addrInfos = addrInfos->ai_next;
		}
		WSACleanup();
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
#endif
		for (int i = 0; i < ip_lst.size(); i++)
		{
			addrs += ip_lst[i];
			if (i < ip_lst.size() - 1)
				addrs += std::string(separator);
		}
		return ip_lst;
	}
}