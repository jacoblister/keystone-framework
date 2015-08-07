/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework_base.c"
#include "objpersistent.c"
#include "objcontainer.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

#include <fcntl.h>
#ifdef OS_Win32
#include <io.h>
#define read(fd, buffer, count) _read(fd, buffer, count)
#define write(fd, buffer, count) _write(fd, buffer, count)
#define lseek(fd, offset, origin) _lseek(fd, offset, origin)
#define open(filename, oflag, pmode) _open(filename, oflag, pmode)
#define close(fd) _close(fd)
#endif
#ifdef OS_Linux
#define O_BINARY 0
#include <sys/ioctl.h>
#include <unistd.h>
#endif
#include <sys/stat.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

class CEvent;
class CHsm;

class CSyncObject : CObjPersistent {
 private:
   bool signaled;
 public:
   static inline bool signaled(void);
   static inline void signal(bool signaled);
};

static inline bool CSyncObject::signaled(void) {
   return this->signaled;
};

static inline void CSyncObject::signal(bool signaled) {
   this->signaled = signaled;
};

/*>>> Should tie in with sync object strucute better.  
      Just use POSIX like interface for now */
class CMutex : CSyncObject {
 private:    
   byte native_data[16];    
 
   void new(void);
   void delete(void);    
 public:
   void CMutex(void);

   int lock(void);
   int trylock(void);
   int unlock(void);
};

ARRAY:typedef<CSyncObject *>;

typedef void (*THREAD_METHOD)(CObject *this, void *data);

class CTimer : CSyncObject {
 private:
   void delete(void);
   int interval_ms;
   THREAD_METHOD method;
   CObject *method_object;
   void *method_data;
   int timer_id;
 public:
   void CTimer(int interval_ms, CObject *object, THREAD_METHOD method, void *data);
};

class CThread : CSyncObject {
 private:
   CTimer *process_timer;
   byte native_data[16];
   THREAD_METHOD method;
   CObject *method_object;
   void *method_data;

   /*>>>event queue*/

   void new(void);

   virtual int entry(void);
 public:
   void CThread(void);

   void send_event(CHsm *receiver, CEvent *event);
   void post_event(CHsm *receiver, CEvent *event);

//   void call(CObject *object, THREAD_METHOD method, void *data);
   bool spawn(const char *process_name);
   void sleep(int time_ms);
   static inline void yield(void);
   CSyncObject* select_object(ARRAY<CSyncObject *> *object, int timeout_ms);
   int wait(void);
   void realtime_set(bool realtime);

   EXCEPTION<CPUFault_Ill, CPUFault_FPE, CPUFault_Segv>;
};

static inline void CThread::yield(void) {
   CThread(this).sleep(0);
}/*CThread::yield*/

class CThreadMain : CThread {
 private:
   void new(void);
};

class CIOObject : CSyncObject {
 private:
   int handle;
 public:
   void CIOObject(void);
   void open_handle(int handle); 
   int open(const char *name, int oflag);
   int close(void);
   static inline int read(void *buf, int count);
   static inline int write(void *buf, int count);
   static inline int length(void);
   static inline off_t lseek(off_t offset, int whence);

   virtual int eof(void);
   virtual int read_data(ARRAY<byte> *buffer, int count);
   virtual int write_data(void *buf, int count);
   virtual void write_string(char *buffer);
   virtual int read_string(CString *string);
   int ioctl(int request, void *argp);
};

static inline int CIOObject::read(void *buf, int count) {
   return read(this->handle, buf, count);
};

static inline int CIOObject::write(void *buf, int count) {
   return write(this->handle, buf, count);
};

static inline int CIOObject::length(void) {
   int result;
   result = lseek(this->handle, 0, SEEK_END);
   lseek(this->handle, 0, SEEK_SET);
   return result;
};

static inline off_t CIOObject::lseek(off_t offset, int whence) {
   return lseek(this->handle, offset, whence);
};

class CSerial : CIOObject {
};

class CIOMemory : CIOObject {
 private:
   byte *data;
   int size;
   int read_position;
 public:
   void CIOMemory(void *data, int size);

   virtual int eof(void);
   virtual int read_data(ARRAY<byte> *buffer, int count);
};

/*--*/

#if 0
class CObjClientStream : CObjClient {
   CStream *stream;
};
#endif

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

void CIOObject::CIOObject(void) {
}/*CIOObject::CIOObject*/

void CIOObject::open_handle(int handle) {
   this->handle = handle;
}/*CIOObject::open_handle*/

int CIOObject::open(const char *name, int oflag) {
   this->handle = name ? open(name, oflag, S_IREAD | S_IWRITE) : STDOUT;
   return this->handle;
}/*CIOObject::open*/

int CIOObject::close(void) {
   return close(this->handle);
}/*CIOObject::close*/

int CIOObject::eof(void) {
   bool result = FALSE;
   long end_pos, cur_pos;

   cur_pos = lseek(this->handle, 0L, SEEK_CUR);

   if (cur_pos < 0)
      return TRUE;

   end_pos = lseek(this->handle, 0L, SEEK_END);

   result = cur_pos >= end_pos;

   lseek(this->handle, cur_pos, SEEK_SET);

   return result;
};

int CIOObject::read_data(ARRAY<byte> *buffer, int count) {
   int read_count = read(this->handle, ARRAY(buffer).data(), count);

//>>>   ARRAY(buffer).used_set(read_count);

   return read_count;
}/*CIOObject::read_data*/

int CIOObject::write_data(void *buf, int count) {
   return CIOObject(this).write(buf, count);
}/*CIOObject::write_data*/

void CIOObject::write_string(char *buffer) {
   write(this->handle, buffer, strlen(buffer));
}/*CIOObject::write_string*/

int CIOObject::read_string(CString *string) {
   return 0; 
}/*CIOObject::read_string*/

#ifdef OS_Linux
int CIOObject::ioctl(int request, void *argp) {
   return ioctl(this->handle, request, argp);
}/*CIOObject::close*/
#endif

int CThread::entry(void) {
   return 0;
}

void CIOMemory::CIOMemory(void *data, int size) {
   this->read_position = 0;
   this->data = data;
   this->size = size;
}/*CIOMemory::CIOMemory*/

int CIOMemory::eof(void) {
   return this->read_position >= this->size;
}/*CIOObject::eof*/

int CIOMemory::read_data(ARRAY<byte> *buffer, int count) {
   if (count + this->read_position > this->size) {
      count = this->size - this->read_position;
   }

   memcpy(ARRAY(buffer).data(), this->data + this->read_position, count);
   this->read_position += count;

   return count;
}/*CIOMemory::read_data*/

OBJECT<CThreadMain, thread_main>;

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/

