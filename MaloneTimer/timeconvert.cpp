#include "timerconvert.h"

int MaloneTime::SecToTimestruct(time_t secs, struct tm *ptm)
{
  localtime_r((const time_t *)&secs, ptm);
  return 1;
}

int MaloneTime::TimestructToString(struct tm sttm, char *time_string)
{
  sprintf(time_string, "%04d-%02d-%02d %02d:%02d:%02d", 
    sttm.tm_year + 1900, sttm.tm_mon + 1, sttm.tm_mday, 
    sttm.tm_hour, sttm.tm_min, sttm.tm_sec);
  return 1;
}

int MaloneTime::SecToString(time_t secs, char *time_string)
{
  struct tm sttm;
  SecToTimestruct(secs, &sttm);
  return TimestructToString(sttm, time_string);
}

int MaloneTime::StringToTimestruct(char *time_string, struct tm* ptm)
{
  // %Y-%m-%d == %F, %H:%M:%S == %T
  //if (NULL == strptime(time_string,"%F %T", ptm))
  if (NULL == strptime(time_string,"%Y-%m-%d %H:%M:%S", ptm))
    return 0;
  return 1;
}

int MaloneTime::TimestructToSecs(struct tm sttm, time_t *psecs)
{
  *psecs = mktime(&sttm);
  return 1;
}

int MaloneTime::StringToSecs(char *time_string, time_t *psecs)
{
  struct tm sttm;
  if (StringToTimestruct(time_string, &sttm) == 0)
    return 0;
  return TimestructToSecs(sttm, psecs);
}

int MaloneTime::CompareTime(char *t1, char *t2)
{
  time_t sec1;
  time_t sec2;
  if (!StringToSecs(t1, &sec1)) 
    return -2;
  if (!StringToSecs(t2, &sec2)) 
    return -2;
	
  if (sec1 > sec2) 
    return 1;
  else if (sec1 == sec2) 
    return 0;
  else
    return -1;
|

/*
int main()
{
    time_t timer = time(0);
    char buff[256] = {0};
    SecToString(timer, buff);
    printf("%s\n", buff);
	time_t timer2;
	StringToSecs(buff, &timer2);
	printf("%lu %lu\n", timer, timer2);
	StringToSecs("2012-11-1 10:41:00", &timer2);
	SecToString(timer2, buff);
	printf("%s\n", buff);
    return 0;
}*/
