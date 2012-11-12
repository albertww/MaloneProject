#include "MaloneLog.h"

const char *CMaloneLog::s_LogStr[] = {"ERROR", "WARN", "DEBUG" };

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
		mkdir(m_CurrentDirectory.c_str(), 0775);
	}
	SetModuleName(moduleName);
	//pthread_mutex_init(&m_LogMutex);
}

CMaloneLog::~CMaloneLog()
{
	Destroy();
	//pthread_mutex_destroy(&m_LogMutex);
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
			m_Logfd[level] = open(m_LogFileName[level].c_str(), 
				O_RDWR | O_CREAT | O_APPEND | O_LARGEFILE, 0640);
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
		::localtime_r((const time_t *)&tv.tv_sec, &sttm);
		
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
			m_Logfd[level] = open(m_LogFileName[level].c_str(), 
				O_RDWR | O_CREAT | O_APPEND | O_LARGEFILE, 0640);
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
		::localtime_r((const time_t *)&tv.tv_sec, &sttm);
		
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


