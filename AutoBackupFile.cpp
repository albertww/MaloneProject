#include "malone_file.h"

BufferedFile::BufferedFile(char *file_name, int buff_size)
    : fd_(-1), file_buffer_(NULL), buffer_size_(buff_size), 
	  last_flush_time_(0), buffer_cursor_(0)
{
  strncpy(file_name_, file_name, kDefaultFileNameLen);
  file_name_[kDefaultFileNameLen - 1] = 0;
}

BufferedFile::~BufferedFile()
{
}

int BufferedFile::Init()
{
  if (buffer_size_ <= 0) buffer_size_ = kDefaultFileBufferLen;
  file_buffer_ = new char[buffer_size_];
  last_flush_time_ = time(0);
  if (file_buffer_)
    return 1;
  return 0;
}

int BufferedFile::Deinit()
{
  delete file_buffer_;
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

void Close()
{
  close(fd_);
  fd_ = -1;
}

int BufferedFile::GetFileLen(off_t *len) const
{
  struct stat buf;
  if (stat(filename, &buf) < 0) 
  {
	return 0;
  }
  *len = buf.st_size;
  return 1;
}

int BufferedFile::GetBufferLen() const
{
  return buffer_cursor_;
}

int BufferedFile::Write(char *buff, int len)
{
  if (len + buffer_cursor > buffer_size_)
  {
    // first flush existing buffer data to file
    if (!Flush())
	  return 0;
	int bytes_processed = 0;
	int bytes_copy = 0;
	while (1)
	{
	  // data remain is longer than buffer so flush to file
	  if (len - bytes_processed >= buffer_size_)
	  {
	    memcpy(file_buffer_, 
		       buff + bytes_processed, 
			   buffer_size_);
		buffer_cursor += buffer_size_;
		bytes_processed += buffer_size_;
		if (!Flush())
		  return 0;
		continue;
	  }
	  // data remain is shorter than buffer so reserve to buffer
	  else
	  {
	    if (len - bytes_processed == 0)
		  return 1;
	    memcpy(file_buffer_, 
		       buff + bytes_processed, 
			   len - bytes_processed);
	    buffer_cursor += len - bytes_processed;
		bytes_processed += len - bytes_processed;
	    return 1;
	  }
	}
  }
  else
  {
    // add to buffer
    memcpy(file_buffer_ + buffer_cursor, buff, len);
    buffer_cursor += len;
	// buffer is full
    if (buffer_cursor >= buffer_size_)
      return Flush();
	return 1;
  }
}

int BufferedFile::Flush()
{
  int bytes_write = 0;
  int total = 0;
  while (1)
  {
    int bytes_write = write(fd_, file_buffer_ + total, buffer_cursor - total);
    if (bytes_write < 0)
	{
      return 0;
	}
    total += bytes_write;
    if (total >= buffer_cursor)
    {
	  buffer_cursor = 0;
      return 1;
    }
  }
}


