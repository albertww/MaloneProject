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

using std::string;
using std::ostringstream;

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

#define DEFAULT_LOG_MODULE_NAME "DefaultModule"

class CMaloneLog
{
public:
	CMaloneLog(const char *moduleName = DEFAULT_LOG_MODULE_NAME);
	~CMaloneLog();
	int LogMessage(int level, const char *file, int line, 
					const char *function, const char *fmt, ...);
	int LogMessageStream(int level, const char *file, int line, 
					const char *function, ostringstream& os);
		
protected:
	void Destroy();
	void SetModuleName(const char *moduleName);
	
protected:
	string m_ModuleName;
	string m_LogFileName[MALONE_LOG_LEVEL_MAX];
	int m_Logfd[MALONE_LOG_LEVEL_MAX];
	//pthread_mutex_t m_LogMutex;
	string m_CurrentDirectory;
	
public:
	static const char *s_LogStr[];
};

#endif
