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
#include "syncobject.c"

#define ZLIB_FILE TRUE

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CFile : CIOObject {
 private:
   void delete(void);
 public:
   void CFile(void);
};

ENUM:typedef<EFileLockMode> {
   {none}, {locked}, {loadOnly}
};

#if ZLIB_FILE 
class CFileGZ : CFile {
 private:
   byte lock_check;
   byte lock_check_read;
   void *gz_handle;
   EFileLockMode lock_mode;
   int position;
   bool open_write;
   void delete(void);
 public:
   void CFileGZ(EFileLockMode lock_mode);

   int open(const char *name, const char *mode);
   int close(void);
   virtual int eof(void);
   virtual int read_data(ARRAY<byte> *buffer, int count);
   virtual int write_data(void *buf, int count);
   virtual void write_string(char *buffer);
   virtual int read_string(CString *string);
};
#endif

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

//#include <sys/ioctl.h>

void CFile::CFile(void) {
   CIOObject(this).CIOObject();
}/*CFile::CFile*/

void CFile::delete(void) {
}/*CFile::delete*/

#if ZLIB_FILE 
#include "../zlib/zlib.h"

void CFileGZ::CFileGZ(EFileLockMode lock_mode) {
   this->lock_mode = lock_mode;
}/*CFileGZ::CFileGZ*/

void CFileGZ::delete(void) {
}/*CFileGZ::delete*/

int CFileGZ::open(const char *name, const char *mode) {
   this->gz_handle = gzopen(name, mode);
   if (strchr(mode, 'w') || strchr(mode, 'W')) {
      this->open_write = TRUE;
   }
   else {
      this->open_write = FALSE;
   }
   this->position = 0;
   this->lock_check = 0; 
   return this->gz_handle ? 1 : -1;
}/*CFileGZ::open*/

int CFileGZ::close(void) {
   int result;
  
   if (this->open_write) {
      gzwrite(this->gz_handle, &this->lock_check, 1); 
   }
   else {
      if (this->lock_mode != EFileLockMode.none && !gzdirect(this->gz_handle)) {
         result = gzclose(this->gz_handle);
         return result != 0 ? result : this->lock_check != this->lock_check_read;
      }
      if (this->lock_mode == EFileLockMode.loadOnly && gzdirect(this->gz_handle)) {
         gzclose(this->gz_handle);
         return -1;
      }
   }
   return gzclose(this->gz_handle);
}/*CFileGZ::close*/

int CFileGZ::eof(void) {
   int erno;
   const char *errmsg;

   errmsg = gzerror(this->gz_handle, &erno);
   return gzeof(this->gz_handle) || erno != 0;
}/*CFileGZ::eof*/

int CFileGZ::read_data(ARRAY<byte> *buffer, int count) {
   int read_count;
   int i;
   int erno;
   const char *errmsg;
   
   read_count = gzread(this->gz_handle, ARRAY(buffer).data(), count);
   errmsg = gzerror(this->gz_handle, &erno);
   if (read_count && CFileGZ(this).eof()) {
      this->lock_check_read = ARRAY(buffer).data()[read_count - 1];
      read_count--;
   }
   if (this->lock_mode != EFileLockMode.none && !gzdirect(this->gz_handle)) {
      for (i = 0; i < read_count; i++) {
//      ARRAY(buffer).data()[i] = ARRAY(buffer).data()[i] ^ (i + this->position);
         ARRAY(buffer).data()[i] = ARRAY(buffer).data()[i] ^ 0xFF;
         this->lock_check ^= ARRAY(buffer).data()[i];
      }
   }
   this->position += read_count;   
   
   return read_count;
}/*CFileGZ::read_data*/

int CFileGZ::write_data(void *buf, int count) {
   char *buffer = (char *)buf;
   ARRAY<byte> data;
   int i, written;

   if (this->lock_mode != EFileLockMode.none) {
      if (!count) {
         return 0;
      }
      ARRAY(&data).new();
      ARRAY(&data).used_set(count);
      for (i = 0; i < count; i++) {
         this->lock_check ^= buffer[i];
         ARRAY(&data).data()[i] = buffer[i] ^ 0xFF;
      }
      written = gzwrite(this->gz_handle, ARRAY(&data).data(), count); 
      this->position += written;
      ARRAY(&data).delete();
   }
   else {
      written = gzwrite(this->gz_handle, buffer, strlen(buffer)); 
   }
   
   return written;
}/*CFileGZ::write_data*/

void CFileGZ::write_string(char *buffer) {
   CFileGZ(this).write_data(buffer, strlen(buffer));
}/*CFileGZ::write_string*/

int CFileGZ::read_string(CString *string) {
   return 0;
}/*CFileGZ::read_string*/

#endif

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/

