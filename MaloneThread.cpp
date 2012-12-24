#include "MaloneThread.h"

MaloneThread::MaloneThread(): m_Threadid(0)
{
	memset(m_Name, 0, sizeof(m_Name));
}

MaloneThread(char *name): m_Threadid(0)
{
	memset(m_Name, 0, sizeof(m_Name));
	strncpy(m_Name, name, THREAD_NAME_LEN);
	m_Name[THREAD_NAME_LEN - 1] = 0;
}
	
void MaloneThread::Start()
{
	pthread_create(&m_Threadid, NULL, MaloneThread::ThreadFunc, this);
}


void * MaloneThread::ThreadFunc(void *param)
{
	if (*m_Name)
	{
		// set the name of this thread
		prctl(PR_SET_NAME, (unsigned long)m_Name);
	}
	MaloneThread *pt = (MaloneThread *)param;
	return pt->Run();
}