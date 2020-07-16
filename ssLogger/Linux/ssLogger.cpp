#include "ssLogger.h"
#include <pthread.h>
#include <queue>

static int LogLevel = LOG_DEBUG;
static bool isInit = false;
static std::queue<std::string> qLog;
static pthread_mutex_t mtx;
static pthread_t logThd;

void *thread(void *)
{
    while (true)
    {
    }
}

void ssLoggerInitialize()
{
    LogLevel = LOG_DEBUG;
    int res = pthread_create(&logThd, NULL, thread, NULL);
}

void SetLoggerLevel(int log_level)
{
    if (log_level < LOG_DEBUG || log_level > LOG_DISASTER)
        LogLevel = LOG_DEBUG;
    else
        LogLevel = log_level;
    return;
}

void OutPutString(LOG_LEVEL l, const char *format, ...)
{
    pthread_mutex_lock(&mtx);
    if (isInit == false)
    {
        printf("ssLogger module is not initialize!\n");
        pthread_mutex_unlock(&mtx);
        throw "ssLogger module is not initialize!";
    }

    std::string log;

    qLog.push(log);
    pthread_mutex_unlock(&mtx);
    return;
}

unsigned int timetick()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_usec / 1000 + tv.tv_sec * 1000;
}

std::string DateTime()
{
    static char timestr[200] = {0};
    struct tm *pTempTm;
    struct timeval time;

    gettimeofday(&time, NULL);
    pTempTm = localtime(&time.tv_sec);
    if (NULL != pTempTm)
    {
        snprintf(timestr, 199, "[%04d-%02d-%02d %02d:%02d:%02d.%03ld]",
                 pTempTm->tm_year + 1900,
                 pTempTm->tm_mon + 1,
                 pTempTm->tm_mday,
                 pTempTm->tm_hour,
                 pTempTm->tm_min,
                 pTempTm->tm_sec,
                 time.tv_usec / 1000);
    }
    return std::string(timestr);
}