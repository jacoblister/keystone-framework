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
   CFile infile;
   CFileGZ outfile;
   byte buf[1000];
   int count;
   
   if (ARRAY(args).count() != 3) {
      printf("Usage - gzlock <infile> <outfile>\n");
	  return -1;
   }
   
   new(&infile).CFile();
   if (CIOObject(&infile).open(CString(&ARRAY(args).data()[1]).string(), O_RDONLY | O_BINARY) == -1) {
      delete(&infile);
	  printf("Error - cannot open input file %s\n", CString(&ARRAY(args).data()[1]).string());
	  return -1;
   }
   
   new(&outfile).CFileGZ(EFileLockMode.locked);
   if (CFileGZ(&outfile).open(CString(&ARRAY(args).data()[2]).string(), "wb") == -1) {
      delete(&infile);
	  delete(&outfile);
	  printf("Error - cannot open outfile file %s\n", CString(&ARRAY(args).data()[2]).string());
	  return -1;
   }
   
   do {
      count = CIOObject(&infile).read(buf, sizeof(buf));
	  CFileGZ(&outfile).write_data(buf, count);	  
   } while (count);
   
   CIOObject(&infile).close();
   CFileGZ(&outfile).close();
   delete(&outfile);   
   delete(&infile);   
   
   return 0;
}/*CGZLock::main*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
