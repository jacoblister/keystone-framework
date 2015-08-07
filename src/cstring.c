
/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/
/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "object.c"

#include "framework_base.c"

/*>>>module problem workaround, should be in implementation! */
#include <stdarg.h>

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

//typedef struct const char *STRING;
//st = STRING.new();
//STRING(st).set(const char *str);
//STRING(st).setn(const char *str, n);
//STRING(st).delete();
//st = STRING.realloc(st);

ENUM:typedef<EStringEncoding> {
   {ASCII}, {UTF16}
};

/*>>>inherit from CLiteObject*/
/*>>>use STL string based string class*/
class CString : CObject {
 private:
   void delete(void);

   EStringEncoding encoding;
   ARRAY<char> data;

 public:
   void CString(const char *str);
   void encoding_set(EStringEncoding encoding);
   void set(const char *str);
   void setn(const char *str, int n);
   void set_string(const CString *str);
   void append(unsigned short character);
   void appendn(const char *str, int n);
   void append_string(const CString *str);
   static inline void clear(void);
   static inline bool empty(void);
   void printf(const char *format, ...);
   void printf_append(const char *format, ...);
   void printf_prepend(const char *format, ...);
   void printf_insert(const char *start, int count, char *format, ...);
   void insert(int after, const char *str);
   void erase(int index, int count);
   void extract(CString *dest, const char *begin, const char *end);
   bool tokenise(CString *dest, const char *delim, bool strip_token);
   static inline char* string(void);
   static inline unsigned short* wstring(void);
   static inline bool match(CString *str);
   static inline int strcmp(const char *str);
   static inline int strncmp(const char *str, int n);
   static inline char *strchr(char c);
   static inline char *strrchr(char c);
   char *strmatch(const char *c);
   static inline int length(void);
   void print(void);
};

ARRAY:typedef<CString>;

static inline void CString::clear(void) {
   ARRAY(&this->data).used_set(0);
}/*CString::clear*/

static inline bool CString::empty(void) {
   return ARRAY(&this->data).count() == 0;
}/*CString::empty*/

static inline char *CString::string(void) {
   if (this->encoding != EStringEncoding.ASCII) {
      //ASSERT("CString::string - wrong encoding)");
     CString(this).encoding_set(EStringEncoding.ASCII);
   }
   return ARRAY(&this->data).count() ? ARRAY(&this->data).data() : "";
}/*CString::string*/

static inline unsigned short *CString::wstring(void) {
//   static unsigned short term = 0;
   if (this->encoding != EStringEncoding.UTF16) {
      ASSERT("CString::wstring - wrong encoding)");
   }
//   return ARRAY(&this->data).count() ? (unsigned short *)ARRAY(&this->data).data() : &term;//L"";
   return ARRAY(&this->data).count() ? (unsigned short *)ARRAY(&this->data).data() : (unsigned short *)"\0\0";
}/*CString::wstring*/

static inline bool CString::match(CString *str) {
   int i;

   if (this->encoding != str->encoding ||
      CString(this).length() != CString(str).length()) {
      return FALSE;
   }

   for (i = 0; i < ARRAY(&this->data).count(); i++) {
      if (ARRAY(&this->data).data()[i] != ARRAY(&str->data).data()[i]) {
         return FALSE;
      }
   }

   return TRUE;
}/*CString::match*/

static inline int CString::strcmp(const char *str) {
   if (this->encoding != EStringEncoding.ASCII) {
      ASSERT("CString::strcmp - wrong encoding)");
   }

   if (CString(this).empty()) {
      return -1;
   }
   else {
      return strcmp(ARRAY(&this->data).data(), str);
   }
}/*CString::strcmp*/

static inline int CString::strncmp(const char *str, int n) {
   if (this->encoding != EStringEncoding.ASCII) {
      ASSERT("CString::strncmp - wrong encoding)");
   }

   if (CString(this).empty()) {
      return -1;
   }
   else {
      return strncmp(ARRAY(&this->data).data(), str, n);
   }
}/*CString::strncmp*/

static inline char *CString::strchr(char c) {
   if (this->encoding != EStringEncoding.ASCII) {
//      ASSERT("CString::strchr - wrong encoding)");
      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   if (!ARRAY(&this->data).data()) {
      return NULL;
   }
   return strchr(ARRAY(&this->data).data(), c);
}/*CString::strchr*/

static inline char *CString::strrchr(char c) {
   if (this->encoding != EStringEncoding.ASCII) {
//      ASSERT("CString::strrchr- wrong encoding)");
      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   if (!ARRAY(&this->data).data()) {
      return NULL;
   }
   return strrchr(ARRAY(&this->data).data(), c);
}/*CString::strrchr*/

static inline int CString::length(void) {
   if (this->encoding != EStringEncoding.ASCII) {
      return ARRAY(&this->data).count() ? ARRAY(&this->data).count() / 2 - 1 : 0;
   }
   else {
      return ARRAY(&this->data).count() ? (strlen(ARRAY(&this->data).data())) : 0;
   }
}/*CString::length*/

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

//>>>why the insanely high limit?*/
#define STATIC_BUFFER_SIZE 50000

#if 0
//CObject *CString::new(void) {
//   ARRAY(&this->data).new();
//   class:base.new();
//   return this;
//};
#endif

void CString::CString(const char *str) {
   ARRAY(&this->data).new();
   CString(this).set(str);
}/*CString::CString*/

void CString::delete(void) {
   ARRAY(&this->data).delete();
}/*CString::delete*/

void CString::encoding_set(EStringEncoding encoding) {
   /* Set encoding for string and change encoding of existing data if present */
   /*>>>asumes little endian encoding */
   int i, count;

   if (this->encoding == EStringEncoding.ASCII && encoding == EStringEncoding.UTF16) {
      count = ARRAY(&this->data).count();
      ARRAY(&this->data).used_set(count * 2);
      for (i = count - 1; i >= 0; i--) {
         ARRAY(&this->data).data()[i * 2] = ARRAY(&this->data).data()[i];
      }
      for (i = 0; i < count; i++) {
         ARRAY(&this->data).data()[i * 2 + 1] = '\0';
      }
   }
   else if (this->encoding == EStringEncoding.UTF16 && encoding == EStringEncoding.ASCII) {
      /*>>>handle high character loss */
      count = ARRAY(&this->data).count() / 2;
      for (i = 0; i < count; i++) {
         ARRAY(&this->data).data()[i] = ARRAY(&this->data).data()[i * 2];
      }
      ARRAY(&this->data).used_set(count);
   }

   this->encoding = encoding;
}/*CString::encoding_set*/

void CString::set(const char *str) {
   this->encoding = EStringEncoding.ASCII;

   if (str) {
      ARRAY(&this->data).data_set(strlen(str) + 1, str);
   }
   else {
      ARRAY(&this->data).used_set(0);
   }
}/*CString::set*/

void CString::setn(const char *str, int n) {
   if (this->encoding) {
//      ASSERT("CString::setn - wrong encoding)");
      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   if (str) {
      ARRAY(&this->data).used_set(n + 1);
      strncpy(ARRAY(&this->data).data(), str, n);
      ARRAY(&this->data).data()[n] = '\0';
   }
}/*CString::setn*/

void CString::set_string(const CString *str) {
   EStringEncoding encoding;

   ARRAY(&this->data).data_set(ARRAY(&str->data).count(), ARRAY(&str->data).data());
   encoding = this->encoding;
   this->encoding = str->encoding;
   CString(this).encoding_set(encoding);
}/*CString::set_string*/

char *CString::strmatch(const char *c) {
   /*>>>quick hack*/
   return CString(this).strchr(']');
}/*CString::strmatch*/

void CString::append(unsigned short character) {
   CString(this).appendn((const char *)&character, this->encoding ? 2 : 1);
}/*CString::append*/

void CString::appendn(const char *str, int n) {
   int used;
   if (str) {
      used = ARRAY(&this->data).count();
      if (used == 0) {
         used = 2;
      }
      ARRAY(&this->data).used_set(used + n);

//      strncpy(&ARRAY(&this->data).data()[used - 2], str, n);
      memcpy(&ARRAY(&this->data).data()[used - 2], str, n);
      ARRAY(&this->data).data()[used + n - 2] = '\0';
      ARRAY(&this->data).data()[used + n - 1] = '\0';
   }
}/*CString::appendn*/

void CString::append_string(const CString *str) {
   EStringEncoding encoding;
   int used;

   encoding = this->encoding;
   this->encoding = str->encoding;
   CString(this).encoding_set(encoding);

   used = CString(this).length() * (this->encoding ? 2 : 1);
   ARRAY(&this->data).used_set(used + ARRAY(&str->data).count());
   memcpy(&ARRAY(&this->data).data()[used], ARRAY(&str->data).data(), ARRAY(&str->data).count());
}/*CString::set_string*/

void CString::printf(const char *format, ...) {
   va_list args;
   char *output = CFramework(&framework).scratch_buffer_allocate();

   if (this->encoding) {
      /* Force encoding back to ASCII */
      this->encoding = 0;
   }

   va_start(args, format);

   vsprintf(output, format, args);
   CString(this).set(output);

   CFramework(&framework).scratch_buffer_release(output);
}/*CString::printf*/

void CString::printf_append(const char *format, ...) {
   va_list args;
   char *output = CFramework(&framework).scratch_buffer_allocate();

   if (this->encoding != EStringEncoding.ASCII) {
      //ASSERT("CString::printf_append - wrong encoding)");
      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   va_start(args, format);

   sprintf(output, "%s", CString(this).string());
   vsprintf(output + strlen(output), format, args);
   CString(this).set(output);

   CFramework(&framework).scratch_buffer_release(output);
}/*CString::printf_append*/

void CString::printf_insert(const char *start, int count, char *format, ...) {
   va_list args;
   char *output = CFramework(&framework).scratch_buffer_allocate();

   if (this->encoding != EStringEncoding.ASCII) {
      //ASSERT("CString::printf_insert - wrong encoding)");
      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   va_start(args, format);

   strncpy(output, CString(this).string(), start - CString(this).string());
   vsprintf(output + (start - CString(this).string()), format, args);
   sprintf(output + strlen(output), "%s", CString(this).string() + (start - CString(this).string() + count));
   CString(this).set(output);

   CFramework(&framework).scratch_buffer_release(output);
}/*CString::printf_append*/

void CString::printf_prepend(const char *format, ...) {
   va_list args;
   char *output = CFramework(&framework).scratch_buffer_allocate();

   if (this->encoding != EStringEncoding.ASCII) {
//      ASSERT("CString::printf_prepend - wrong encoding)");
       CString(this).encoding_set(EStringEncoding.ASCII);
   }

   va_start(args, format);

   vsprintf(output, format, args);
   sprintf(output + strlen(output), "%s", CString(this).string());
   CString(this).set(output);

   CFramework(&framework).scratch_buffer_release(output);
}/*CString::printf_prepend*/

void CString::insert(int after, const char *str) {
   char *cp;

   if (this->encoding) {
//      ASSERT("CString::insert - wrong encoding)");
      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   ARRAY(&this->data).used_set(ARRAY(&this->data).count() + strlen(str));
   cp = CString(this).string();

   memmove(cp + after + 1 + strlen(str), cp + after + 1, strlen(cp + after + 1) + 1);
   memcpy(cp + after + 1, str, strlen(str));
}/*CString::insert*/

void CString::erase(int index, int count) {
   char *cp = CString(this).string() + index;

   if (this->encoding) {
//      ASSERT("CString::erase - wrong encoding)");
      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   memmove(cp, cp  + 1, strlen(cp + 1) + 1);
   ARRAY(&this->data).used_set(ARRAY(&this->data).count() - count);
}/*CString::erase*/

void CString::extract(CString *dest, const char *begin, const char *end) {
   if (this->encoding) {
//      ASSERT("CString::extract - wrong encoding)");
      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   if (end < begin)
     ASSERT("CString::end < begin)");

   ARRAY(&dest->data).used_set(end - begin + 2);
   memmove(ARRAY(&dest->data).data(), begin, end - begin + 1);
//      ARRAY(&dest->data).data_set(end - begin + 2, begin);
   ARRAY(&dest->data).data()[end - begin + 1] = '\0';
}/*CString::extract*/

bool CString::tokenise(CString *dest, const char *delim, bool strip_token) {
   char *end = NULL;
   bool token = FALSE;

   if (this->encoding) {
//      ASSERT("CString::tokenise - wrong encoding)");
      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   if (!CString(this).length()) {
      CString(dest).clear();
      return FALSE;
   }

   end = CString(this).string() + (strip_token ? 0 : 1);
   while (*(end + 1)) {
      if (strchr(delim, *end)) {
         end -= (strip_token ? 0 : 1);
         token = TRUE;
         break;
      }
      end++;
   }

   CString(this).extract(dest, CString(this).string(), end);
   if (token && strip_token) {
      CString(dest).setn(CString(dest).string(), CString(dest).length() - 1);
   }
   CString(this).set(strlen(end) ? end + 1 : end);

   return TRUE;
}/*CString::tokenise*/

void CString::print(void) {
   if (this->encoding) {
//      ASSERT("CString::print - wrong encoding)");
      CString(this).encoding_set(EStringEncoding.ASCII);
   }

   printf("%s\n", CString(this).string());
}/*CString::print*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/