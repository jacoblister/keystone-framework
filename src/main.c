/*! \mainpage
 \section intro Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
 */

/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

/*>>>move all system specific stuff out of this module*/
#include "framework_base.c"

//#include "thread.h"

#include "cstring.c"
#include "datetime.c"
#include "objpersistent.c"
#include "serverstream.c"
#include "objcontainer.c"
#include "syncobject.c"
#include "file.c"
#include "memfile.c"
#include "graphics/graphics.c"
#include "extras/extras.c"

/*>>>not implemented*/
#if 0
CClassLib *CLASSLIB_new(CClassLib *this);
bool CLASSLIB_load(CClassLib *this, CObjClass *base, char *filename);
#endif

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

/*>>>for now*/
ARRAY:typedef<CString>;

class CApplication : CObjPersistent {
 private:
   void new(void);
   void delete(void);
   ATTRIBUTE<CString default_help, "defaultHelp">;
   ATTRIBUTE<CString home_path, "homePath">;
   void NATIVE_set_home_path(char *argv0);
 public:

   void CApplication(void);
   virtual int main(ARRAY<CString> *args);
   virtual bool notify_event(CObject *dest, CEvent *event);
};

/*>>>not implemented*/
#if 0
CClassLib *CLASSLIB_new(CClassLib *this);
bool CLASSLIB_load(CClassLib *this, CObjClass *base, char *filename);
#endif

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#include "../version.h"

/* force static linking */
CObjClass *class_primitive = &class(CGPrimitive);
CObjClass *class_primcontainer = &class(CGPrimContainer);
CObjClass *class_animinput = &class(CGAnimateInput);

extern CMemFileDirectory fw_memfile_directory;

int CApplication::main(ARRAY<CString> *args) {
   return 0;
}/*CApplication::main*/

bool CApplication::notify_event(CObject *dest, CEvent *event) {
   return FALSE;
}/*CApplication::notify_event*/

void CApplication::new(void) {
   new(&this->default_help).CString(NULL);
   new(&this->home_path).CString(NULL);
}/*CApplication::new*/

void CApplication::CApplication(void) {
}/*CApplication::CApplication*/

void CApplication::delete(void) {
   delete(&this->home_path);
   delete(&this->default_help);
}/*CApplication::delete*/

extern char *app_initdata_begin, *app_initdata_end, *app_uninitdata_begin;

CFramework framework;
CMutex framework_scratch_mutex;

static byte keystone_scratch_buffer[KEYSTONE_SCRATCH_BUFFERS][65536];

void *CFramework::scratch_buffer_allocate(void) {
   int i;
   void *result = NULL;

   CMutex(&framework_scratch_mutex).lock();

   for (i = 0; i < KEYSTONE_SCRATCH_BUFFERS; i++) {
      if (!this->scratch_buffer[i]) {
         this->scratch_buffer[i] = keystone_scratch_buffer[i];
         result = this->scratch_buffer[i];
         break;
      }
   }

   CMutex(&framework_scratch_mutex).unlock();

   if (!result) {
      ASSERT("FRAMEWORK:Out of scratch bufferes");
   }

   return result;
}/*CFramework::scratch_buffer_allocate*/

void CFramework::scratch_buffer_release(void *buffer) {
   int i;
   CMutex(&framework_scratch_mutex).lock();

   for (i = 0; i < KEYSTONE_SCRATCH_BUFFERS; i++) {
      if (buffer == this->scratch_buffer[i]) {
         this->scratch_buffer[i] = NULL;
         break;
      }
   }

   if (i == KEYSTONE_SCRATCH_BUFFERS) {
      ASSERT("FRAMEWORK:scratch buffer release failed");
   }

   CMutex(&framework_scratch_mutex).unlock();
}/*CFramework::scratch_buffer_release*/

//void *CFramework::static_buffer(void) {
//    /*>>>klunky not thread safe shared big temp buffer */
//    static byte framework_buffer[65536];
//    return framework_buffer;
//}/*CFramework::static_buffer*/

CObjClass *CFramework::obj_class_find(const char *class_name) {
   CObjClass *obj_class;
   int i;
   const char *cp;

   for (i = 0; i < ARRAY(&framework.obj_class).count(); i++) {
      obj_class = ARRAY(&framework.obj_class).data()[i];

      if (strcmp(obj_class->name, class_name) == 0 ||
         (obj_class->alias && strcmp(obj_class->alias, class_name) == 0))
         return obj_class;
   }

   /* Second pass, attempt best match for class name with no namespace */
   /* >>>potentally dangereous and could return unexpected results,
      probably should not be on by default */
   for (i = 0; i < ARRAY(&framework.obj_class).count(); i++) {
      obj_class = ARRAY(&framework.obj_class).data()[i];

      cp = strchr(class_name, ':');
      cp = cp ? cp + 1 : class_name;

      if (strcmp(CObjClass_alias_short(obj_class), cp/*class_name*/) == 0)
         return obj_class;
   }

   return NULL;
}/*CFramework::obj_class_find*/

void CFramework::obj_class_list_all(void) {
   CObjClass *obj_class;
   int i;

   for (i = 0; i < ARRAY(&framework.obj_class).count(); i++) {
      obj_class = ARRAY(&framework.obj_class).data()[i];
      printf("%s\n", obj_class->name);
   }
}/*CFramework::obj_class_list_all*/

void CFramework::initdata_search(void) {
   char *dp, *data_end;
//   TObjClassDefEnd *cdef_end;

   data_end = (char *)&app_uninitdata_begin;

   dp = (char *)&app_initdata_begin;

   while (dp < data_end) {
      if (*((unsigned int *)dp) == SID_INIT) {
         switch (((TInit *)dp)->type) {
         case IT_Class:
            this->obj_class_current = (CObjClass *)dp;
            ARRAY(&this->obj_class).item_add(this->obj_class_current);
            break;
         case IT_ObjectDef:
            ARRAY(&this->auto_object).item_add((TObjectDef *)dp);
            break;
         default:
            break;
         }
      }
      dp += sizeof(unsigned int);
   }
}/*CFramework::initdata_search*/

CApplication *CFramework::construct_auto_object(void) {
   TObjectDef *obj_def;
   CApplication *application = NULL;
   int i;

   for (i = 0; i < ARRAY(&framework.auto_object).count(); i++) {
      obj_def = ARRAY(&framework.auto_object).data()[i];

      /*test if this is an application object*/
      if (CObjClass_is_derived(&class(CApplication), obj_def->obj_class)) {
         application = (CApplication *)obj_def->obj;
      }

      /* create the application object */
      new(obj_def->obj).class(obj_def->obj_class);

      /*>>>hack! pass data as object linkage*/
      if (obj_def->data) {
         CObject((CObject *)obj_def->obj).notify_object_linkage(EObjectLinkage.ParentSet, (void *)obj_def->data);
      }
   }

   return application;
}/*CFramework::construct_auto_object*/

void CFramework::new(void) {
   int i;
   TObjectDef *odef;

   new(&fw_memfile_directory).CMemFileDirectory();
   this->memfile_directory = &fw_memfile_directory;

   ARRAY(&framework.obj_class).new();
   ARRAY(&framework.auto_object).new();
   ARRAY(&framework.window).new();

   CFramework(this).initdata_search();

   /* create automatic objects */
   for (i = 0; i < ARRAY(&framework.auto_object).count(); i++) {
      odef = ARRAY(&framework.auto_object).data()[i];
   }



   CFramework(this).NATIVE_new();

   new(&framework_scratch_mutex).CMutex();
}/*CFramework::new*/

void CFramework::delete(void) {
   int i;
   TObjectDef *odef;

   delete(&framework_scratch_mutex);

   CFramework(this).NATIVE_delete();

   for (i = 0; i < ARRAY(&framework.auto_object).count(); i++) {
      odef = ARRAY(&framework.auto_object).data()[i];
//      OBJ_delete(odef->object);
      odef++;
   }

   ARRAY(&framework.window).delete();
   ARRAY(&framework.auto_object).delete();
   ARRAY(&framework.obj_class).delete();

//   PCLASSLIST_delete(&framework.pclass_list);
}/*CFramework::delete*/

void CFramework::license_info(char *result) {
   sprintf(result, "%s", VERSION_STRING"\n");
   sprintf(result + strlen(result), "Platform: ");
#if OS_Win32
   sprintf(result + strlen(result), "Win32\n");
#endif
#if OS_Linux
   sprintf(result + strlen(result), "GNU/Linux\n");
#endif
   sprintf(result + strlen(result), "Copyright (c) 2006, Jacob Lister, Abbey Systems\n");
   sprintf(result + strlen(result), "Rights reserved, refer to\n");
   sprintf(result + strlen(result), "http:/");
   sprintf(result + strlen(result), "/www.keystoneframework.org/license.html\n");
}/*CFramework::license_info*/

void CFramework::license_components(char *result) {
   sprintf(result,
            "Expat v1.95.7\n"
            );
}/*CFramework::license_components*/

void CFramework::main(void) {
   if (framework.executing)
     ASSERT("FRAMEWORK_execute:Framework already executing");

#if 1 // OS_PalmOS
/*>>>quick hack till exception handling working */
   framework.executing = TRUE;
   CFramework(this).NATIVE_main();
   framework.executing = FALSE;
#else
   /*>>>exception handling not working right*/
   try {
      framework.executing = TRUE;
      CFramework(this).NATIVE_main();
   }
   catch(NULL, EXCEPTION<CGWindow,NoWindows>) {
   }
   finally {
      framework.executing = FALSE;
   }
#endif
}/*CFramework::main*/

void CFramework::kill(void) {
   if (!framework.executing)
     ASSERT("FRAMEWORK_kill:Framework not executing");
   CFramework(this).NATIVE_kill();
}/*CFramework::kill*/

int CFramework::execute(int argc, char *argv[]) {
   ARRAY<CString> args;
   CString *string;
   int i, result;

   ARRAY(&args).new();
   for (i = 0; i < argc; i++) {
      /*>>>item_add_empty return pointer?*/
      ARRAY(&args).item_add_empty();
      string = ARRAY(&args).item_last();
      new(string).CString(argv[i]);
   }

   framework.application = CFramework(&framework).construct_auto_object();

   if (argc) {
      CApplication(framework.application).NATIVE_set_home_path(argv[0]);
   }

   if (!framework.application) {
      ASSERT("FRAMEWORK: Application Object not found");
   }

   result = CApplication(framework.application).main(&args);

   for (i = 0; i < ARRAY(&args).count(); i++) {
      delete(CString(&ARRAY(&args).data()[i]));
   }
   ARRAY(&args).delete();

//   return result;
   exit(result);
}/*CFramework::execute*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/

