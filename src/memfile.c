/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/

/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "objpersistent.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

typedef struct {
   const char *name;
   int size;
   void *data;
} TMemFileDef;

class CMemFile : CObjPersistent {
 private:
   void new(void);
   void delete(void);

   TMemFileDef *memfile_def;

   void notify_object_linkage(EObjectLinkage linkage, CObject *object);
 public:
   ATTRIBUTE<CString name>;
};

ARRAY:typedef<CMemFile *>;

class CMemFileDirectory : CObjPersistent {
 private:
   ARRAY<CMemFile *> memfile;

   void new(void);
   void delete(void);
 public:
   void CMemFileDirectory(void);

   void memfile_add(CMemFile *memfile);
   CMemFile *memfile_find(const char *name);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

CMemFileDirectory fw_memfile_directory;

void CMemFileDirectory::new(void) {
   ARRAY(&this->memfile).new();
}/*CMemFileDirectory::new*/

void CMemFileDirectory::CMemFileDirectory(void) {
}/*CMemFileDirectory::CMemFileDirectory*/

void CMemFileDirectory::delete(void) {
   ARRAY(&this->memfile).delete();
}/*CMemFileDirectory::delete*/

void CMemFileDirectory::memfile_add(CMemFile *memfile) {
   ARRAY(&this->memfile).item_add(memfile);
}/*CMemFileDirectory::memfile_add*/

CMemFile *CMemFileDirectory::memfile_find(const char *name) {
   CMemFile *memfile;
   int i;

   for (i = 0; i < ARRAY(&this->memfile).count(); i++) {
      memfile = ARRAY(&this->memfile).data()[i];
      if (strcmp(memfile->memfile_def->name, name) == 0) {
         return memfile;
      }
   }

   return NULL;
}/*CMemFileDirectory::memfile_find*/

void CMemFile::new(void) {
}/*CMemFile::new*/

void CMemFile::delete(void) {
}/*CMemFile::delete*/

void CMemFile::notify_object_linkage(EObjectLinkage linkage, CObject *object) {
   this->memfile_def = (TMemFileDef *)object;

   CMemFileDirectory(&fw_memfile_directory).memfile_add(this);
}/*CMemFile::notify_object_linkage*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/

