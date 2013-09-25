#include "MaloneLog.h"
#ifdef _WINDOWS
#include <direct.h>
#endif
#include "timeconvert.h"
#include <memory.h>
#include <errno.h>
#include <dirent.h> // dirent
#include <vector>
#include <algorithm>

using namespace std;

CMaloneLog g_Log;

int EnumFilename(const char *path, vector<std::string> &file_list)
{
  DIR *handle = opendir(path);
  if (NULL == handle)
    return 0;
  
  struct dirent *iterator;
  while (NULL != (iterator = readdir(handle)))
  {
    file_list.push_back(iterator->d_name);
  }
  
  return file_list.size();
}

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

// --------------------------------------------------------------------

template <class T>
CThreadSafeQueue<T>::CThreadSafeQueue()
{
}

template <class T>
CThreadSafeQueue<T>::~CThreadSafeQueue()
{
	pthread_cond_destroy(&m_Cond);
	pthread_mutex_destroy(&m_Mutex);
}

template <class T>
int CThreadSafeQueue<T>::Init()
{
	int ret = 0;
	ret = pthread_mutex_init(&m_Mutex, NULL);
	if (ret != 0)
	{
		return 0;
	}

	ret = pthread_cond_init(&m_Cond, NULL);
	if (ret != 0)
	{
		pthread_mutex_destroy(&m_Mutex);
		return 0;
	}
	return 1;
}

template <class T>
int CThreadSafeQueue<T>::Push(const T &data)
{
	pthread_mutex_lock(&m_Mutex);	
	m_Queue.push_back(data);
	pthread_mutex_unlock(&m_Mutex);
	pthread_cond_signal(&m_Cond);
	return 1;
}

template <class T>
int CThreadSafeQueue<T>::Pop(T &data)
{
	pthread_mutex_lock(&m_Mutex);
	while (m_Queue.size() == 0)
	{
		pthread_cond_wait(&m_Cond, &m_Mutex);
	}
	data = *(m_Queue.begin());
	m_Queue.pop_front();
	pthread_mutex_unlock(&m_Mutex);
	return 1;
}

template <class T>
int CThreadSafeQueue<T>::Size()
{
	return m_Queue.size();
}

// ---------------------------------------------------------------

const char *CMaloneLog::s_LogStr[] = {"ERROR", "WARN", "DEBUG" };
CMaloneLog *CMaloneLog::s_Logger = NULL;

CMaloneLog::CMaloneLog():m_CacheFileNum(CACHE_FILE_NUM)
{
	
}

CMaloneLog::~CMaloneLog()
{
	Destroy();
	//pthread_mutex_destroy(&m_LogMutex);
}

void CMaloneLog::RemoveTimeoutLogFile()
{
	for (int i = MALONE_LOG_LEVEL_ERROR; i < MALONE_LOG_LEVEL_MAX; ++i)
	{
		string matchFileName = m_ModuleName + "_" + s_LogStr[i] + ".log.";
		LOGPrintf("matchfile %s\n", matchFileName.c_str());
		vector<string> filelist;
		EnumFilename(m_CurrentDirectory.c_str(), filelist);
		LOGPrintf("total %i\n", filelist.size());
		vector<string>::iterator it = filelist.begin();
		while (it != filelist.end())
		{
			if ((strstr(it->c_str(), matchFileName.c_str())))
			{
				it++;
			}
			else
			{
				LOGPrintf("%s filterd\n", it->c_str());
				filelist.erase(it);
			}
		}
		LOGPrintf("total %i\n", filelist.size());
		sort(filelist.begin(), filelist.end());
		reverse(filelist.begin(), filelist.end());
		it = filelist.begin();
		while (it != filelist.end())
		{
			LOGPrintf("%s matched\n", it->c_str());
			it++;
		}
		if (filelist.size() > m_CacheFileNum)
		{
			for (int ind = filelist.size() - 1; ind >= m_CacheFileNum; ind--)
			{
				string deleteFile = m_CurrentDirectory + "/" + filelist[ind].c_str();
				unlink(deleteFile.c_str());
				LOGPrintf("%s unlink\n", filelist[ind].c_str());
			}
		}
	}
}

int CMaloneLog::Init(const char *moduleName)
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
	
	if (!m_Queue.Init())
		return 0;
		
	pthread_create(&m_WriteThread, NULL, CMaloneLog::ThreadFunc, (void *)this);
	
	return 1;
}

int CMaloneLog::WriteToDisk(DiskMessage *pmsg)
{
	int retval = 0;
	int level = pmsg->Level;
	do
	{
		
		if (level > MALONE_LOG_LEVEL_DEBUG || level < 0)
			break;
		
		if (m_Logfd[level] < 0)
		{
		#if defined(_LINUX)
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
		
		int size = strlen(pmsg->String) + 1;
		int count = 0;
		
		while (size)
		{
			int writeCount = write(m_Logfd[level], pmsg->String + count, size - count);
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
		LOGPrintf("log file is %s\n", m_LogFileName[level].c_str());
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
			LOGPrintf("len > 1000 rename from %s to %s result is %d errno is %d\n", m_LogFileName[level].c_str(), newname, suc, errno);
			
			RemoveTimeoutLogFile();
		}
	}
	
	return retval;
}

int CMaloneLog::Process()
{
	while (1)
	{
		bool out = false;
		DiskMessage *pmsg;
		if (m_Queue.Pop(pmsg))
		{
			if (pmsg->Flag == THREAD_STOP)
			{
				delete pmsg;
				break;
			}
			WriteToDisk(pmsg);
			delete pmsg->String;
			delete pmsg;
		}
	}
	
	return 0;
}

void * CMaloneLog::ThreadFunc(void *param)
{
	CMaloneLog *plog = (CMaloneLog *)(param);
	if (plog)
		plog->Process();

	return 0;
}

CMaloneLog * CMaloneLog::sharedLogger()
{
	if (s_Logger == NULL)
	{
		s_Logger = &g_Log;
		if (s_Logger)
		{
			if (!s_Logger->Init("shishicai"))
			{
				delete s_Logger;
				s_Logger = NULL;
			}
		}
	}
	return s_Logger;
}

int CMaloneLog::LogMessage(int level, const char *file, int line, 
						const char *function, const char *fmt, ...)
{
	int retval = 0;
	
	do
	{
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
		
		DiskMessage *pmsg = new DiskMessage;
		pmsg->String = new char[size];
		memcpy(pmsg->String, buffer, size);
		pmsg->Level = level;
		
		m_Queue.Push(pmsg);
		
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
		#if defined(_LINUX)
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
		
		DiskMessage *pmsg = new DiskMessage;
		pmsg->String = new char[size];
		memcpy(pmsg->String, buffer, size);
		pmsg->Level = level;
		
		m_Queue.Push(pmsg);
    } while(0);
	
	return retval;
}

#if 0
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
		#if defined(_LINUX)
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
		LOGPrintf("log file is %s\n", m_LogFileName[level].c_str());
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
			LOGPrintf("len > 1000 rename from %s to %s result is %d errno is %d\n", m_LogFileName[level].c_str(), newname, suc, errno);
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
		#if defined(_LINUX)
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
		LOGPrintf("log file is %s\n", m_LogFileName[level].c_str());
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
			LOGPrintf("len > 1000 rename from %s to %s result is %d errno is %d\n", m_LogFileName[level].c_str(), newname, suc, errno);
		}
	}
	
	return retval;
}

#endif
		
void CMaloneLog::Destroy()
{
	DiskMessage *pmsg = new DiskMessage;
	pmsg->Flag = THREAD_STOP;
	m_Queue.Push(pmsg);
	void *value;
	pthread_join(m_WriteThread, &value);
	
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



