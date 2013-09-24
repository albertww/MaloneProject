#ifndef MALONE_TIME_H_
#define MALONE_TIME_H_

#include <time.h>

class MaloneTime
{
public:
  static int SecToTimestruct(time_t secs, struct tm *ptm);

  static int TimestructToString(struct tm sttm, char *time_string);
  static int TimestructToCompactString(struct tm sttm, char *time_string);

  static int SecToString(time_t secs, char *time_string);

  static int SecToCompactString(time_t secs, char *time_string);

  static int StringToTimestruct(char *time_string, struct tm* ptm);

  static int TimestructToSecs(struct tm sttm, time_t *psecs);

  static int StringToSecs(char *times_tring, time_t *psecs);
  
  // compare two string of datetime
  // @return 1 if t1 > t2, 0 if t1 == t2, -1 if t1 < t2
  //         -2 if t1 or t2 is invalid
  static int CompareTime(char *t1, char *t2);
};

#endif
