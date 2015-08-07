/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework.c"

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

//CObjClass *sqlclient = &class(CSQLClient);

class CGWindowSQLTest : CGWindow {
 public:
   void CGWindowSQLTest(const char *title, CGCanvas *canvas, CGWindow *parent);
   bool notify_request_close(void);
};

void CGWindowSQLTest::CGWindowSQLTest(const char *title, CGCanvas *canvas, CGWindow *parent) {
   CGWindow(this).CGWindow(title, canvas, parent);
}/*CGWindowSQLTest::CGWindowSQLTest*/

bool CGWindowSQLTest::notify_request_close(void) {
   if (ARRAY(&framework.window).count() <= 1) {
      CFramework(&framework).kill();
   }
   
   return TRUE;
}/*CGWindowSQLTest::notify_request_close*/

class CSQLTestApp : CApplication {
 private:
   CGLayoutSQLTest layout[2];
   CGWindowSQLTest window[2];
   CSQLClientLocal sql_client;
 public:
   void main(ARRAY<CString> *args);
};

OBJECT<CSQLTestApp, sql_test_app>;

int CSQLTestApp::main(ARRAY<CString> *args) {
   const char *db_name = NULL, *db_query = NULL, *db_error = NULL;
   new(&this->sql_client).CSQLClientLocal();
   
   if (ARRAY(args).count() >= 2) {
      db_name = CString(&ARRAY(args).data()[1]).string();
   }
   if (ARRAY(args).count() >= 3) {
      db_query = CString(&ARRAY(args).data()[2]).string();
   }
   
   if (CSQLClient(&this->sql_client).open(db_name) != 0) {
      db_error = "Cannot Open Database";
   }
   
   new(&this->layout[0]).CGLayoutSQLTest(CSQLClient(&this->sql_client), db_query, db_error);
   new(&this->window).CGWindowSQLTest("SQL Test", CGCanvas(&this->layout[0]), NULL);
   CGWindow(&this->window[0]).show(TRUE);
   
   CFramework(&framework).main();

   CSQLClient(&this->sql_client).close();
   
   delete(&this->sql_client);   
   
   return 0;
}/*CSQLTestApp::main*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
