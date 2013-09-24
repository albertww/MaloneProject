#include "MaloneLog.h"
#ifdef _WINDOWS
#include <direct.h>
#endif
#include "timeconvert.h"
#include <memory.h>
#include <errno.h>

#define CACHE_SIZE 100000000

int FileExist(const char *filename)
{
  if (0 == access(filename, F_OK))
    return 1;
  return 0;
}

int GetFileLen(const char *filename, off_t *len)
{
  struct stat buf;
  if (stat(filename, &buf) < 0) 
  {
	return 0;
  }
  *len = buf.st_size;
  return 1;
}

int Rename(const char *oldname, const char *newname)
{
  if (0 == rename(oldname, newname))
    return 1;
  return 0;
}

const char *CMaloneLog::s_LogStr[] = {"ERROR", "WARN", "DEBUG" };
CMaloneLog *CMaloneLog::s_Logger = NULL;

CMaloneLog::CMaloneLog(const char *moduleName)
{
	for (int index = 0; index < MALONE_LOG_LEVEL_MAX; index++)
	{
		m_Logfd[index] = -1;
	}
	char buffer[1024];
	m_CurrentDirectory = getcwd(buffer, sizeof(buffer));
	m_CurrentDirectory += "/log";
	if (access(m_CurrentDirectory.c_str(), F_OK) < 0)
	{
	#ifndef _WINDOWS
		mkdir(m_CurrentDirectory.c_str(), 0775);
	#else
		mkdir(m_CurrentDirectory.c_str());
	#endif
	}
	SetModuleName(moduleName);
	//pthread_mutex_init(&m_LogMutex);
}

CMaloneLog::~CMaloneLog()
{
	Destroy();
	//pthread_mutex_destroy(&m_LogMutex);
}

CMaloneLog * CMaloneLog::sharedLogger()
{
	if (s_Logger == NULL)
	{
		s_Logger = new CMaloneLog("shishicai");
		
	}
	return s_Logger;
}

int CMaloneLog::LogMessage(int level, const char *file, int line, 
						const char *function, const char *fmt, ...)
{
	int retval = 0;
	
	do
	{
		if (level > MALONE_LOG_LEVEL_DEBUG || level < 0)
			break;
		
		if (m_Logfd[level] < 0)
		{
		#ifndef _WINDOWS
			m_Logfd[level] = open(m_LogFileName[level].c_str(), 
				O_RDWR | O_CREAT | O_APPEND | O_LARGEFILE, 0640);
		#else
			m_Logfd[level] = open(m_LogFileName[level].c_str(), 
				O_RDWR | O_CREAT | O_APPEND, 0640);
		#endif
			if (m_Logfd[level] == -1)
			{
				retval = -1;
				break;
			}
		}
		
		struct timeval tv;
		struct timezone tz;
		gettimeofday(&tv, &tz);
		struct tm sttm;
		#ifndef _WINDOWS
		::localtime_r((const time_t *)&tv.tv_sec, &sttm);
		#else
		struct tm *ptm;
		ptm = localtime((const time_t *)&tv.tv_sec);
		sttm = *ptm;
		#endif
		
		char message[4096];
		char buffer[5120];
		
		va_list args;
		va_start(args, fmt);
		vsnprintf(message, sizeof(message), fmt, args);
		va_end(args);
		
		int size = snprintf(buffer, sizeof(buffer), 
			"[%04d-%02d-%02d %02d:%02d:%02d.%d] %-5s (%s : %d : %s) %s\n",
			sttm.tm_year + 1900, sttm.tm_mon + 1, sttm.tm_mday,
			sttm.tm_hour, sttm.tm_min, sttm.tm_sec, tv.tv_usec,
			s_LogStr[level], file, line, function, message);
			
		while (buffer[size - 2] == '\n')
			size--;
		buffer[size] = '\0';
		int count = 0;
		
		while (size)
		{
			int writeCount = write(m_Logfd[level], buffer + count, size - count);
			if (writeCount < 0)
				break;
			count += writeCount;
			if (count >= size)
			{
				retval = 1;
				break;
			}
		}
    } while(0);
	
	if (FileExist(m_LogFileName[level].c_str()))
  	{
		printf("log file is %s\n", m_LogFileName[level].c_str());
		off_t filelen;
		GetFileLen(m_LogFileName[level].c_str(), &filelen);
		if (filelen > CACHE_SIZE)
		{
			close(m_Logfd[level]);
			m_Logfd[level] = -1;
			char newname[256];
			memset(newname, 0, sizeof(newname));
			char timestr[32];
			time_t now = time(NULL);
			MaloneTime::SecToCompactString(now, timestr);
			sprintf(newname, "%s.%s", m_LogFileName[level].c_str(), timestr);
			int suc = Rename(m_LogFileName[level].c_str(), newname);
			printf("len > 1000 rename from %s to %s result is %d errno is %d\n", m_LogFileName[level].c_str(), newname, suc, errno);
		}
	}
	
	return retval;
}

int CMaloneLog::LogMessageStream(int level, const char *file, int line, 
					const char *function, ostringstream& os)
{
	string str = os.str();
	
	int retval = 0;
	
	do
	{
		if (level > MALONE_LOG_LEVEL_DEBUG || level < 0)
			break;
		
		if (m_Logfd[level] < 0)
		{
		#ifndef _WINDOWS
			m_Logfd[level] = open(m_LogFileName[level].c_str(), 
				O_RDWR | O_CREAT | O_APPEND | O_LARGEFILE, 0640);
		#else
			m_Logfd[level] = open(m_LogFileName[level].c_str(), 
				O_RDWR | O_CREAT | O_APPEND, 0640);
		#endif
			if (m_Logfd[level] == -1)
			{
				retval = -1;
				break;
			}
		}
		
		struct timeval tv;
		struct timezone tz;
		gettimeofday(&tv, &tz);
		struct tm sttm;
		#ifndef _WINDOWS
		::localtime_r((const time_t *)&tv.tv_sec, &sttm);
		#else
		struct tm *ptm;
		ptm = localtime((const time_t *)&tv.tv_sec);
		sttm = *ptm;
		#endif
		
		char buffer[5120];
		
		int size = snprintf(buffer, sizeof(buffer), 
			"[%04d-%02d-%02d %02d:%02d:%02d.%d] %-5s (%s : %d : %s) %s\n",
			sttm.tm_year + 1900, sttm.tm_mon + 1, sttm.tm_mday,
			sttm.tm_hour, sttm.tm_min, sttm.tm_sec, tv.tv_usec,
			s_LogStr[level], file, line, function, str.c_str());
			
		while (buffer[size - 2] == '\n')
			size--;
		buffer[size] = '\0';
		int count = 0;
		
		while (size)
		{
			int writeCount = write(m_Logfd[level], buffer + count, size - count);
			if (writeCount < 0)
				break;
			count += writeCount;
			if (count >= size)
			{
				retval = 1;
				break;
			}
		}
    } while(0);
	
	if (FileExist(m_LogFileName[level].c_str()))
  	{
		printf("log file is %s\n", m_LogFileName[level].c_str());
		off_t filelen;
		GetFileLen(m_LogFileName[level].c_str(), &filelen);
		if (filelen > CACHE_SIZE)
		{
			close(m_Logfd[level]);
			m_Logfd[level] = -1;
			char newname[256];
			memset(newname, 0, sizeof(newname));
			char timestr[32];
			time_t now = time(NULL);
			MaloneTime::SecToCompactString(now, timestr);
			sprintf(newname, "%s.%s", m_LogFileName[level].c_str(), timestr);
			int suc = Rename(m_LogFileName[level].c_str(), newname);
			printf("len > 1000 rename from %s to %s result is %d errno is %d\n", m_LogFileName[level].c_str(), newname, suc, errno);
		}
	}
	
	return retval;
}
		
void CMaloneLog::Destroy()
{
	for (int index = 0; index < MALONE_LOG_LEVEL_MAX; index++)
	{
		if (m_Logfd[index] >= 0)
			close(m_Logfd[index]);
		m_Logfd[index] = -1;
	}
}

void CMaloneLog::SetModuleName(const char *moduleName)
{
	if (moduleName == NULL || *moduleName == '\0')
		m_ModuleName = DEFAULT_LOG_MODULE_NAME;
	else
		m_ModuleName = moduleName;
		
	for (int index = 0; index < MALONE_LOG_LEVEL_MAX; index++)
	{
		m_LogFileName[index] = m_CurrentDirectory + "/" + m_ModuleName + "_" + s_LogStr[index] + ".log";
	}
}



