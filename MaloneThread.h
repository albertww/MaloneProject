

#include <pthread.h>
#include <sys/prctl.h>

#define THREAD_NAME_LEN 64

class MaloneThread
{
public:
	MaloneThread();
	MaloneThread(char *name);
	
	virtual void * Run() {}
	void Start();
	static void * ThreadFunc(void *param);
	
protected:
	pthread_t m_Threadid;
	char m_Name[THREAD_NAME_LEN];
};