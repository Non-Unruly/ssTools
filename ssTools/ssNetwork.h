#ifndef _SS_NETWORK_H_
#define _SS_NETWORK_H_

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "ssTools.h"

#ifdef _WIN32
// windows
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#else
//linux
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

namespace ssTools
{
	std::vector<std::string> GetLocalIPv4(std::string &addrs, const char* separator = ",");
};

#endif

