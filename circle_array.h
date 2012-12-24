
#ifndef CIRCLE_ARRAY_H_
#define CIRCLE_ARRAY_H_

#include <stdlib.h>
#include <unistd.h>

template <class T>
class CircleArray
{
public:
  CircleArray(int maxsize): max_array_len_(maxsize),
                            array_(NULL),
                            write_cursor(0),
                            read_cursor(0)
  {
    if (max_array_len_ <= 0)
	  max_array_len_ = kDefaultArrayLen;
	Init();
  }
	
  virtual ~CircleArray() 
  {
    Deinit();
  }
  
  int Init()
  {
    if (NULL == array_)
	  array_ = new T[max_array_len_];
	if (NULL != array_)
	  return 1;
	return 0;
  }
  
  int Deinit()
  {
    delete array_;
	array_ = NULL;
	return 1;
  }
  
  inline int Size()
  {
    return write_cursor_ - read_cursor_;
  }
  
  inline int SpaceRemain()
  {
    return max_array_len_ - (write_cursor_ - read_cursor_)
  }
  
  int Put(T value)
  {
    if (SpaceRemain() > 0)
	{
	  Check();
	  array_[write_cursor_ % max_array_len_] = value;
	  ++write_cursor_;
	  return 1;
	}
	return 0;
  }
  
  /*
    put several values into array, if space remain is shorter
	than number of values, only part of values can be put in
    @return, number of values be put into array 
  */
  int PutN(T *values, int number)
  {
    if (number <= 0)
	  return 0;
	int number_puts = min(number, SpaceRemain());
	for (int i = 0; i < number_puts; ++i)
	  Put(values[i]);
	return number_puts;
  }
  
  int Get(T &value)
  {
    if (Size() > 0)
	{
	  value = array_[read_cursor_ % max_array_len_];
	  ++read_cursor_;
	  return 1;
	}
	return 0;
  }
  
  int GetN(T *values, int number)
  {
    if (number <= 0)
	  return 0;
	int number_gets = min(number, Size());
	for (int i = 0; i < number_gets; ++i)
	  Get(values[i]);
	return number_gets;
  }
  
  inline void Check()
  {
    // in order to keep cursor not overflow when it's too big we will turn it down
    if (write_cursor_ >= 0xFFFFFF00)
	{
	  printf("check w %i r %i\n", write_cursor_, read_cursor_);
	  if (write_cursor_ == read_cursor_)
	  {
	    printf("check same, set w to 0 r to 0\n");
		write_cursor_ = write_cursor_ % max_array_len_;
		read_cursor_ = write_cursor_;
	  }
	  else
	  {
	    write_cursor_ = write_cursor_ % max_array_len_;
		read_cursor_ = read_cursor_ % max_array_len_;
	  }
	}
  }
	
protected:
  unsigned int max_array_len_;
  T *array_;
  unsigned int write_cursor_;
  unsigned int read_cursor_;
  
public:
  static const int kDefaultArrayLen = 1000;
};

void RandomTest()
{
  const int len = 5;
  CircleArray<int> ca(len);
  int values[len];
  int value = 0;
  int outvalue = 0;
  srand(time(0));
  for (int i = 0; i < 100; i++)
  {
    int op = rand() % 4;
    switch(op)
    {
    case 0:
      if (ca.Put(value))
      {
        printf("put %i size %i remain %i\n", value, ca.Size(), ca.SpaceRemain());
        value++;
      }
      else
      {
        printf("put failed size %i remain %i\n", ca.Size(), ca.SpaceRemain());
      }
      break;
    case 1:
      {
        int templen = 0;
        while (templen <= 0)
        {
          templen = rand() % len + 1;
        }
        char str[256] = {0};
        for (int i = 0; i < templen; i++)
          values[i] = value++;
        int puts = ca.PutN(values, templen);
        if (puts < templen)
          value -= templen - puts;
        for (int i = 0; i < puts; i++)
          sprintf(strlen(str) + str, " %i ", values[i]);
        if (puts > 0)
          printf("puts %s size %i remain %i\n", str, ca.Size(), ca.SpaceRemain());
        else
          printf("puts failed\n");
      }
      break;
    case 2:
      if (ca.Get(outvalue))
      {
        printf("get %i size %i remain %i\n", outvalue, ca.Size(), ca.SpaceRemain());
      }
      else
      {
        printf("get failed size %i remain %i\n", ca.Size(), ca.SpaceRemain());
      }
      break;
    case 3:
      {
        int templen = 0;
        while (templen <= 0)
        {
          templen = rand() % len + 1;
        }
        int gets = ca.GetN(values, templen);
        char str[256] = {0};
        for (int i = 0; i < gets; i++)
          sprintf(strlen(str) + str, " %i ", values[i]);
        if (gets > 0)
          printf("gets %s size %i remain %i\n", str, ca.Size(), ca.SpaceRemain());
        else
          printf("gets failed\n");
      }
      break;
    }
  }
}
int main()
{
  RandomTest();
  
  return 1;
}

#endif