#include "malone_file.h"
#include <dirent.h> // dirent

// -------------------------------------------------------

int FileUtility::EnumFilename(const char *path, vector<std::string> &file_list)
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

int FileUtility::GetFileLen(const char *filename, off_t *len)
{
  struct stat buf;
  if (stat(filename, &buf) < 0) 
  {
	return 0;
  }
  *len = buf.st_size;
  return 1;
}

int FileUtility::FileExist(const char *filename)
{
  if (0 == access(filename, F_OK))
    return 1;
  return 0;
}

int FileUtility::Rename(const char *oldname, const char *newname)
{
  if (0 == rename(oldname, newname))
    return 1;
  return 0;
}

int FileUtility::CopyFile(const char *srcfile, const char *dstfile)
{
  int srcfd = open(srcfile, O_RDONLY);
  if (-1 == srcfd)
    return 0;
  int dstfd = open(dstfile, O_WRONLY | O_CREAT | O_EXCL, 0640);
  if (-1 == dstfd)
    return 0;
  
  int retval = 0;
  size_t total = 0;
  size_t bytes_read = 0;
  const int kBuffLen = 4096;
  char buff[kBuffLen];
  while (1)
  {
    bytes_read = read(srcfd, buff, kBuffLen);
	// error happen
	if (bytes_read < 0)
	{
	  break;
	}
	// read end
	else if (bytes_read == 0)
	{
	  retval = 1;
	  break;
	}
	// read ok
	else
	{
	  int bytes_write = write(dstfd, buff, bytes_read);
	  if (bytes_write < 0 || bytes_write != bytes_read)
	  {
	    break;
	  }
	}
  }
  
  close(srcfd);
  close(dstfd);
  return retval;
}

// -----------------------------------------------------------------

BufferedFile::BufferedFile(const char *file_name, int buff_size)
    : fd_(-1), file_buffer_(NULL), buffer_size_(buff_size), 
	  last_flush_time_(0), buffer_cursor_(0)
{
  strncpy(file_name_, file_name, kDefaultFileNameLen - 1);
  if (buffer_size_ <= 0) 
    buffer_size_ = kDefaultFileBufferLen;
  file_buffer_ = new char[buffer_size_];
  last_flush_time_ = time(0);
}

BufferedFile::~BufferedFile()
{
  Close();
  delete file_buffer_;
}


// change the filename
// if the older file is opened, first flush data, then close it
// and reset the filename
void BufferedFile::SetFilename(const char *filename)
{
  Close();
  strncpy(file_name_, filename, kDefaultFileNameLen - 1);
}
	
int BufferedFile::Open()
{
  return Open(O_RDWR | O_CREAT | O_APPEND | O_LARGEFILE, 0640);
}

int BufferedFile::Open(int flags, mode_t mod)
{
  fd_ = open(file_name_, flags, mod);
  if (fd_ >= 0) 
	return 1;
  else
    return 0;
}

void BufferedFile::Close()
{
  Flush();
  close(fd_);
  fd_ = -1;
}

int BufferedFile::GetFileLen(off_t *len) const
{
  off_t file_len = 0;
  *len = 0;
  if (0 == GetDiskFileLen(&file_len))
    return 0;
  file_len += buffer_cursor_;
  *len = file_len;
  return 1;
}

int BufferedFile::GetDiskFileLen(off_t *len) const
{
  return FileUtility::GetFileLen(file_name_, len);
}

int BufferedFile::GetBufferLen(off_t *len) const
{
  *len = buffer_cursor_;
  return 1;
}

int BufferedFile::Write(char *buff, int len)
{
  if (fd_ == -1 || len <= 0)
    return 0;
  int total_process = 0;
  int need_process = len; 
  while (need_process > 0)
  {
    int buff_remain = buffer_size_ - buffer_cursor_;
    int bytes_copy = buff_remain >= need_process ? need_process : buff_remain;
    memcpy(file_buffer_ + buffer_cursor_,
           buff + total_process,
           bytes_copy);
    total_process += bytes_copy;
    buffer_cursor_ += bytes_copy;
    need_process -= bytes_copy;
    if (buffer_cursor_ == buffer_size_)
    {
      if (!Flush())
        return 0;
    }
  }
  return total_process;
}  


int BufferedFile::Flush()
{
  if (fd_ == -1)
    return 0;
  last_flush_time_ = time(0);
  int bytes_write = 0;
  int total = 0;
  while (1)
  {
    int bytes_write = write(fd_, file_buffer_ + total, buffer_cursor_ - total);
    if (bytes_write < 0)
	{
      return 0;
	}
    total += bytes_write;
    if (total >= buffer_cursor_)
    {
	  buffer_cursor_ = 0;
      return 1;
    }
  }
}

// ----------------------------------------------------------------------------

#if 0
AutoBackupFile::AutoBackupFile(char *file_name)
{
  
}

AutoBackupFile::~AutoBackupFile()
{
}

#endif

void PrintBuffFile(BufferedFile &bfile)
{
  
  off_t file_len = 0;
  off_t buff_len = 0;
  off_t disk_len = 0;
  bfile.GetDiskFileLen(&disk_len);
  bfile.GetBufferLen(&buff_len);
  bfile.GetFileLen(&file_len);
  printf("file len is %llu, disk %llu, buff %llu\n", file_len, disk_len, buff_len);
}

int main()
{
  // test FileUtility
#if 0
  vector<std::string> vf;
  int len = FileUtility::EnumFilename("./", vf);
  int i = 0;
  for (; i < len; i++)
    printf("%s  ", vf[i].c_str());
  printf("\n");
  
  FileUtility::CopyFile("malone_file.cpp" , "txt");
  if (FileUtility::FileExist("txt"))
    printf("copy ok\n");
  else
    printf("copy failed\n");

  off_t file_len = 0;
  if (FileUtility::GetFileLen((const char *)"txt", &file_len))
    printf("file len is %llu\n", file_len);

  if (FileUtility::Rename("txt", "txt_"))
    printf("rename ok\n");
#endif
  BufferedFile bfile("hello.txt", 64);
  bfile.Open();
  char buff[8192];
  memset(buff, 'a', sizeof(buff));
  for (int i = 0; i < 5; ++i)
  {
    bfile.Write2(buff, i * 30);
    PrintBuffFile(bfile);
  }

  //bfile.Close();
  return 0;
}


