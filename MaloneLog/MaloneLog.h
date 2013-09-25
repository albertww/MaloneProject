#ifndef __MALONE_LOG_H__
#define __MALONE_LOG_H__

#include <stdarg.h>
#include <time.h>
#include <stdio.h>
#include <sys/time.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ostream>
#include <sstream>
#include <pthread.h>
#include <list>

using std::string;
using std::ostringstream;
using std::list;

#define MALONE_LOG_LEVEL_ERROR 0
#define MALONE_LOG_LEVEL_WARN 1
#define MALONE_LOG_LEVEL_DEBUG 2
#define MALONE_LOG_LEVEL_MAX 3
#define MALONE_LOG_LEVEL(level) MALONE_LOG_LEVEL_##level, __FILE__, __LINE__, __FUNCTION__
#define MALONE_LOG(logger, level, ...) logger.LogMessage(MALONE_LOG_LEVEL(level), __VA_ARGS__)

#define MALONE_ERROR_LOG(logger, ...) logger.LogMessage(MALONE_LOG_LEVEL(ERROR), __VA_ARGS__)
#define MALONE_WARN_LOG(logger, ...) logger.LogMessage(MALONE_LOG_LEVEL(WARN), __VA_ARGS__)
#define MALONE_DEBUG_LOG(logger, ...) logger.LogMessage(MALONE_LOG_LEVEL(DEBUG), __VA_ARGS__)

#define MALONE_ERROR_LOGS(logger, logmsg) {\
	ostringstream os; \
	os << logmsg; \
	logger.LogMessageStream(MALONE_LOG_LEVEL(ERROR), os); \
}
#define MALONE_WARN_LOGS(logger, logmsg) {\
	ostringstream os; \
	os << logmsg; \
	logger.LogMessageStream(MALONE_LOG_LEVEL(WARN), os); \
}
#define MALONE_DEBUG_LOGS(logger, logmsg) {\
	ostringstream os; \
	os << logmsg; \
	logger.LogMessageStream(MALONE_LOG_LEVEL(DEBUG), os); \
}

// you can use below log macro
// the following is c style

#define LOG_WARN(...) MALONE_WARN_LOG((*CMaloneLog::sharedLogger()), __VA_ARGS__);
#define LOG_DEBUG(...) MALONE_DEBUG_LOG((*CMaloneLog::sharedLogger()), __VA_ARGS__);
#define LOG_ERROR(...) MALONE_ERROR_LOG((*CMaloneLog::sharedLogger()), __VA_ARGS__);

// the following is cpp stream style

#define LOG_WARN_S(logmsg) MALONE_WARN_LOGS((*CMaloneLog::sharedLogger()), logmsg);
#define LOG_DEBUG_S(logmsg) MALONE_DEBUG_LOGS((*CMaloneLog::sharedLogger()), logmsg);
#define LOG_ERROR_S(logmsg) MALONE_ERROR_LOGS((*CMaloneLog::sharedLogger()), logmsg);

#define DEFAULT_LOG_MODULE_NAME "DefaultModule"

#define CACHE_SIZE 1000
//#define CACHE_SIZE 100000000
#define CACHE_FILE_NUM 5

#ifndef _DEBUG
#define LOGPrintf
#else
#define LOGPrintf printf
#endif

#define THREAD_GOING 1
#define THREAD_STOP 2
struct DiskMessage
{
	DiskMessage()
	{
		Flag = THREAD_GOING;
		Level = MALONE_LOG_LEVEL_MAX;
		String = NULL;
	}
	int Flag;
	int Level;
	char *String;
};

template <class T>
class CThreadSafeQueue
{
public:
	CThreadSafeQueue();
	virtual ~CThreadSafeQueue();
	int Init();
	// @param data to be put in queue
	// @return 1 suc, 0 failed
	virtual int Push(const T &data);
	// @param data, fetch the next ipaddr
	// @return 1 suc, 0 failed
	virtual int Pop(T &data);
	int Size();
protected:
	list<T> m_Queue;
	pthread_mutex_t m_Mutex;
	pthread_cond_t m_Cond;
};

class CMaloneLog
{
public:
	CMaloneLog();
	~CMaloneLog();
	int Init(const char *moduleName = DEFAULT_LOG_MODULE_NAME);
	void RemoveTimeoutLogFile();
	int LogMessage(int level, const char *file, int line, 
					const char *function, const char *fmt, ...);
	int LogMessageStream(int level, const char *file, int line, 
					const char *function, ostringstream& os);
	
protected:
	int WriteToDisk(DiskMessage *pmsg);
	int Process();
	void Destroy();
	void SetModuleName(const char *moduleName);
	
protected:
	string m_ModuleName;
	string m_LogFileName[MALONE_LOG_LEVEL_MAX];
	int m_Logfd[MALONE_LOG_LEVEL_MAX];
	int m_CacheFileNum;
	//pthread_mutex_t m_LogMutex;
	pthread_t m_WriteThread;
	CThreadSafeQueue<DiskMessage *> m_Queue;
	string m_CurrentDirectory;
	
public:
	static const char *s_LogStr[];
	static CMaloneLog * sharedLogger();
	static CMaloneLog *s_Logger;
	static void * ThreadFunc(void *param);
};


#endif
