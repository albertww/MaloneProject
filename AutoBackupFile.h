
#ifndef MALONE_FILE_H_
#define MALONE_FILE_H_

#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class BufferedFile
{
public:
  BufferedFile(char *file_name, int buff_size = kDefaultFileBufferLen);
  virtual ~BufferedFile();
  
  int Init();
  int Deinit();
  
  // open file by default, O_RDWR | O_CREAT | O_APPEND, 0640
  int Open();
  int Open(int flags, mode_t mod);
  void Close();
  
  int GetFileLen(off_t *len) const;
  int GetBufferLen() const;
  int Write(char *buff, int len);
  int Flush();
  
public:
  static const int kDefaultFileNameLen = 256;
  static const int kDefaultFileBufferLen = 4096;


protected:
  int fd_;
  char file_name_[kDefaultFileNameLen];
  char *file_buffer_;
  int buffer_size_;
  int buffer_cursor_;
  time_t last_flush_time_;
};

class AutoBackupFile
{
public:
  AutoBackupFile();
  int EnumBackupFilename(char *file_name_list);
  
  static const int kFileLength = 1000000000;
protected:
  
};

#endif