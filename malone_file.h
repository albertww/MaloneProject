
#ifndef MALONE_FILE_H_
#define MALONE_FILE_H_

#include <string>
#include <vector>

#include <unistd.h> // access
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h> // rename

using namespace std;
// use design pattern chain of responsibility

class FileUtility
{
public:
  static int EnumFilename(const char *path, vector<std::string> &file_list);
  static int GetFileLen(const char *filename, off_t *len);
  static int FileExist(const char *filename);
  static int Rename(const char *oldname, const char *newname);
  static int CopyFile(const char *srcfile, const char *dstfile);
};

class BufferedFile
{
public:
  BufferedFile(const char *file_name, int buff_size = kDefaultFileBufferLen);
  virtual ~BufferedFile();
  
  void SetFilename(const char *filename);
  
  // open file by default, O_RDWR | O_CREAT | O_APPEND, 0640
  int Open();
  int Open(int flags, mode_t mod);
  void Close();
  
  // get the file length, consist of disk file length and
  // the data buffer length
  // @param len, store the file length
  // @return 0, failed, 1 succ
  int GetFileLen(off_t *len) const;
  // get the disk file length
  // @param len, store the file length
  // @return 0, failed, 1 succ
  int GetDiskFileLen(off_t *len) const;
  // get the data buffer length, it's the data not flush to
  // disk yet but reserved in the memory
  // @return the buffer length
  int GetBufferLen(off_t *len) const;
  int Write(char *buff, int len);
  int Flush();
  
public:
  static const int kDefaultFileNameLen = 256;
  static const int kDefaultFileBufferLen = 4096;

protected:
  int fd_;
  char file_name_[kDefaultFileNameLen];
  char *file_buffer_;
  size_t buffer_size_;
  off_t buffer_cursor_;
  time_t last_flush_time_;
};

#if 0
class AutoBackupFile
{
public:
  AutoBackupFile(char *file_name);
  virtual ~AutoBackupFile();
  int Open();
  int Open(int flags, mode_t mod);
  void Close();
  int Write(char *buff, int len);
  int Flush();
  
protected:
  int EnumBackupFilename(char *path, vector<std::string> &file_list);
  int RelocateFile();
  
  static const int kFileLength = 1000000;
protected:
  
};
#endif

#endif
