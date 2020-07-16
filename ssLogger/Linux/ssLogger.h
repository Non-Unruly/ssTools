#ifndef _SS_LOGGER_H_
#define _SS_LOGGER_H_

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <unistd.h>

enum LOG_LEVEL
{
    LOG_DEBUG = 1,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_DISASTER,
};

extern "C"
{
    void SetLoggerLevel(int log_level);

    void ssLoggerInitialize();

    void OutPutString(LOG_LEVEL l, const char *format, ...);

    //毫秒级时间戳
    unsigned int timetick();

    //时间格式化，精确到毫秒
    std::string DateTime();
}

#endif