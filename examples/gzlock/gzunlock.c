/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CGZLock : CApplication {
 public:
   void main(ARRAY<CString> *args);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

OBJECT<CGZLock, helloworldapp>;

int CGZLock::main(ARRAY<CString> *args) {
   CFileGZ infile;
   CFile outfile;
   ARRAY<byte> buf;
   int count;
   
   if (ARRAY(args).count() != 3) {
      printf("Usage - gzunlock <infile> <outfile>\n");
	  return -1;
   }
   
   new(&infile).CFileGZ(EFileLockMode.locked);
   if (CFileGZ(&infile).open(CString(&ARRAY(args).data()[1]).string(), "rb") == -1) {
      delete(&infile);
	  printf("Error - cannot open input file %s\n", CString(&ARRAY(args).data()[1]).string());
	  return -1;
   }
   
   new(&outfile).CFile();
   if (CIOObject(&outfile).open(CString(&ARRAY(args).data()[2]).string(), O_WRONLY | O_CREAT | O_TRUNC | O_BINARY) == -1) {
      delete(&infile);
	  delete(&outfile);
	  printf("Error - cannot open outfile file %s\n", CString(&ARRAY(args).data()[2]).string());
	  return -1;
   }
   
   ARRAY(&buf).new();
   ARRAY(&buf).used_set(1000);
   
   do {
      count = CFileGZ(&infile).read_data(&buf, 1000);
	  CIOObject(&outfile).write(ARRAY(&buf).data(), count);
   } while (count);
   
   CFileGZ(&infile).close();
   CIOObject(&outfile).close();
   
   ARRAY(&buf).delete();
   delete(&outfile);   
   delete(&infile);   
   
   return 0;
}/*CGZLock::main*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
