#include <time.h>

int SecToTimestruct(time_t secs, struct tm *ptm);

int TimestructToString(struct tm sttm, char *timestring);

int SecToString(time_t secs, char *timestring);

int StringToTimestruct(char *timestring, struct tm* ptm);

int TimestructToSecs(struct tm sttm, time_t *psecs);

int StringToSecs(char *timestring, time_t *psecs);