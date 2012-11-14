#include "timerconvert.h"

int SecToTimestruct(time_t secs, struct tm *ptm)
{
    localtime_r((const time_t *)&secs, ptm);
    return 1;
}

int TimestructToString(struct tm sttm, char *timestring)
{
    sprintf(timestring, "%04d-%02d-%02d %02d:%02d:%02d", sttm.tm_year + 1900,
        sttm.tm_mon + 1, sttm.tm_mday, sttm.tm_hour,
        sttm.tm_min, sttm.tm_sec);
    return 1;
}

int SecToString(time_t secs, char *timestring)
{
    struct tm sttm;
    SecToTimestruct(secs, &sttm);
    return TimestructToString(sttm, timestring);
}

int StringToTimestruct(char *timestring, struct tm* ptm)
{
    // %Y-%m-%d == %F, %H:%M:%S == %T
    //if (NULL == strptime(timestring,"%F %T", ptm))
    if (NULL == strptime(timestring,"%Y-%m-%d %H:%M:%S", ptm))
        return 0;
    return 1;
}

int TimestructToSecs(struct tm sttm, time_t *psecs)
{
	*psecs = mktime(&sttm);
	return 1;
}

int StringToSecs(char *timestring, time_t *psecs)
{
	struct tm sttm;
	if (StringToTimestruct(timestring, &sttm) == 0)
		return 0;
	return TimestructToSecs(sttm, psecs);
}

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
