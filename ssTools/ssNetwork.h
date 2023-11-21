#ifndef _SS_NETWORK_H_
#define _SS_NETWORK_H_

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "ssTools.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#else
#endif

namespace ssTools
{
	std::string GetLocalIPv4(std::vector<std::string> &ip_lst=std::vector<std::string>());
};

#endif

