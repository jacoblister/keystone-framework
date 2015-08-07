/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/
/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

/*>>>don't include time.h in interface */
#include <time.h>

#include "framework_base.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

/* >>>more time types */
/* Unix time type >>>should not use time_t ideally*/
//typedef time_t TUNIXTime;
//typedef unsigned long TUNIXTime;

ATTRIBUTE:typedef<TUNIXTime>;

ENUM:typedef<EUNIXTimePart> {
   {Year},
   {Month},
   {Day},
   {Hour},
   {Minute},
   {Second},
   {DayOfWeek},
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#include "objpersistent.c"

extern time_t mktime_gmt(struct tm *ptm);

/*
  TIME attribute indexes:
  none - full time
  0    - year
  1    - month
  2    - day
  3    - hour
  4    - minute
  5    - second
  6    - day of week
*/

bool ATTRIBUTE:convert<TUNIXTime>(CObjPersistent *object,
                                  const TAttributeType *dest_type, const TAttributeType *src_type,
                                  int dest_index, int src_index,
                                  void *dest, const void *src) {
   TUNIXTime *value;
   int int_value;
   time_t value_time;
   CString *string;
   struct tm *timeinfo, mk_timeinfo;
   int result, dow;

   if (dest_type == &ATTRIBUTE:type<TUNIXTime> && src_type == NULL) {
      memset(dest, 0, sizeof(TUNIXTime));
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<TUNIXTime> && src_type == &ATTRIBUTE:type<TUNIXTime>) {
      *((TUNIXTime *)dest) = *((TUNIXTime *)src);
      return TRUE;
   }
   if (dest_type == NULL && src_type == &ATTRIBUTE:type<TUNIXTime>) {
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TUNIXTime> && src_type == &ATTRIBUTE:type<int>) {
      value = (TUNIXTime *)dest;
      value_time = *value;
      if (dest_index != -1) {
         timeinfo = gmtime(&value_time);
         switch (dest_index){
         case EUNIXTimePart.DayOfWeek:
            break;
         case EUNIXTimePart.Year:
            timeinfo->tm_year = *((int *)src) - 1900;
            break;
         case EUNIXTimePart.Month:
            timeinfo->tm_mon = *((int *)src) - 1;
            break;
         case EUNIXTimePart.Day:
            timeinfo->tm_mday = *((int *)src);
            break;
         case EUNIXTimePart.Hour:
            timeinfo->tm_hour = *((int *)src);
            break;
         case EUNIXTimePart.Minute:
            timeinfo->tm_min = *((int *)src);
            break;
         case EUNIXTimePart.Second:
            timeinfo->tm_sec = *((int *)src);
            break;
         }
         value_time = mktime_gmt(timeinfo);
         *value = (TUNIXTime)value_time;

         if (dest_index == EUNIXTimePart.DayOfWeek) {
            dow = timeinfo->tm_wday;
            /* adjust to Monday, rather than Sunday centric */
            dow = (dow + 6) % 7;

            *value -= dow * (60 * 60 * 24);
            *value += (*((int *)src) - 1) * (60 * 60 * 24);
         }
      }
      else {
         *((TUNIXTime *)dest) = *((int *)src);
      }
      return TRUE;
   }
   if (dest_type == &ATTRIBUTE:type<int> && src_type == &ATTRIBUTE:type<TUNIXTime>) {
      value = (TUNIXTime *)src;
      value_time = *value;
      timeinfo = gmtime(&value_time);
      if (src_index != -1) {
         switch (src_index){
         case EUNIXTimePart.Year:
            result = timeinfo->tm_year + 1900;
            break;
         case EUNIXTimePart.Month:
            result = timeinfo->tm_mon + 1;
            break;
         case EUNIXTimePart.Day:
            result = timeinfo->tm_mday;
            break;
         case EUNIXTimePart.Hour:
            result = timeinfo->tm_hour;
            break;
         case EUNIXTimePart.Minute:
            result = timeinfo->tm_min;
            break;
         case EUNIXTimePart.Second:
            result = timeinfo->tm_sec;
            break;
         default:
            result = 0;
            break;
         }
         *((int *)dest) = result;
      }
      else {
         *((int *)dest) = *((TUNIXTime *)src);
      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<TUNIXTime> && src_type == &ATTRIBUTE:type<CString>) {
      value  = (TUNIXTime *)dest;
      value_time = *value;
      string = (CString *)src;

      CString(string).encoding_set(EStringEncoding.ASCII);

      if (!CString(string).length()) {
         *value = 0;
      }
      else {
         if (dest_index != -1) {
            int_value = atoi(CString(src).string());
            return ATTRIBUTE:convert<TUNIXTime>(object, &ATTRIBUTE:type<TUNIXTime>, &ATTRIBUTE:type<int>,
                                                dest_index, src_index,
                                                dest, (const void *)&int_value);
         }
         else {
            if (!CString(string).strchr('-') && !CString(string).strchr(':')) {
               /* treat string as UNIX timestamp */
               *value = atoi(CString(string).string());
            }
            else {
               CLEAR(&mk_timeinfo);
               sscanf(CString(string).string(),
                     "%4d-%02d-%02d %02d:%02d:%02d",
                     &mk_timeinfo.tm_year, &mk_timeinfo.tm_mon, &mk_timeinfo.tm_mday,
                     &mk_timeinfo.tm_hour, &mk_timeinfo.tm_min, &mk_timeinfo.tm_sec);

               mk_timeinfo.tm_year -= 1900;
               mk_timeinfo.tm_mon -= 1;
               if (mk_timeinfo.tm_year <= 0 || mk_timeinfo.tm_mon < 0) {
                  /* can't interpret, don't change value */
               }
               else {
                  value_time = mktime_gmt(&mk_timeinfo);
                  *value = (TUNIXTime)value_time;
               }
            }
         }
      }
      return TRUE;
   }

   if (dest_type == &ATTRIBUTE:type<CString> && src_type == &ATTRIBUTE:type<TUNIXTime>) {
      value = (TUNIXTime *)src;
      value_time = *value;
      string = (CString *)dest;

      if (*value == 0) {
         CString(string).clear();
      }
      else {
         timeinfo = gmtime(&value_time);
         if (src_index != -1) {
            switch (src_index){
            case EUNIXTimePart.Year:
               result = timeinfo->tm_year + 1900;
               break;
            case EUNIXTimePart.Month:
               result = timeinfo->tm_mon + 1;
               break;
            case EUNIXTimePart.Day:
               result = timeinfo->tm_mday;
               break;
            case EUNIXTimePart.Hour:
               result = timeinfo->tm_hour;
               break;
            case EUNIXTimePart.Minute:
               result = timeinfo->tm_min;
               break;
            case EUNIXTimePart.Second:
               result = timeinfo->tm_sec;
               break;
            default:
               result = 0;
               break;
            }
            CString(string).printf(src_index == 0 ? "%04d" : "%02d", result);
         }
         else {
            CString(string).printf("%4d-%02d-%02d %02d:%02d:%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
                                   timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
         }
      }
      return TRUE;
   }
   return FALSE;
}

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/

