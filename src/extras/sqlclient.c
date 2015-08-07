/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

#include "sqlconn.h"
//possible delegate syntax? */
//DELEGATE:typedef<void SQL_NOTIFY_RESULT(int result, int rows, int columns, const char **data)>;
//DELEGATE<SQL_NOTIFY_RESULT> del;
//DELEGATE(&del).new(CObjPersistent(this), &CSQLClient::result);
//DELEGATE(&del).exec(0, 0, 0, NULL);

//typedef void (*SQL_NOTIFY_RESULT)(void *object, int result, int rows, int columns, const char **data);

typedef struct {
   CObject *notify_object;
   SQL_NOTIFY_RESULT notify_method;
   bool notify_abort;
} TSQLExecRequest;

ARRAY:typedef<TSQLExecRequest>;
ARRAY:typedef<TSQLExecRequest *>;

class CSQLClient : CObjPersistent {
 private:
   struct SQLConn *sqlconn;
   void new(void);   
   void delete(void);
 public:
   void CSQLClient(void);
   virtual int open(const char *db_filename);
   virtual TSQLExecRequest *exec(const char *sql_statement, CObject *notify_object, SQL_NOTIFY_RESULT notify_method);
   virtual int close(void);
   static inline void exec_abort(TSQLExecRequest *exec_request);
   
   ATTRIBUTE<CString database_name>;
};

static inline void CSQLClient::exec_abort(TSQLExecRequest *exec_request) {
   exec_request->notify_abort = TRUE;
}/*CSQLClient::exec_abort*/

class CSQLClientLocal : CSQLClient {
 private:
   void new(void);   
   void delete(void);
   struct SQLConn *sqlconn_server;   
 
   ARRAY<byte> client_message;
   ARRAY<byte> server_message;
   void write_client_message(int count, const byte *data);
   void write_server_message(int count, const byte *data);
   
   CObject *notify_object;
   SQL_NOTIFY_RESULT notify_method;
 public:
   void CSQLClientLocal(void); 
   virtual int open(const char *db_filename);
   virtual TSQLExecRequest *exec(const char *sql_statement, CObject *notify_object, SQL_NOTIFY_RESULT notify_method);
   virtual int close(void);
};

/* Example user interface */

#include "ggrid.c"

class CGLayoutSQLTest : CGLayout {
 private:
   void new(void);
   void delete(void);
 
   CObjServer obj_server;    
   CSQLClient *sql_client;
   
   void notify_sql_result(int result, int rows, int columns, const char **data);
   bool notify_request_close(void);
 
   CGGridEdit *grid;
 public:
   void CGLayoutSQLTest(CSQLClient *client, const char *db_query, const char *db_error);
   void notify_sql_result(int result, int rows, int columns, const char **data);
 
   ATTRIBUTE<CString database_name>;   
   ATTRIBUTE<CString sql_query>;
   ATTRIBUTE<bool sql_execute> {
      CSQLClient(this->sql_client).exec(CString(&this->sql_query).string(), 
                                        CObject(this), (SQL_NOTIFY_RESULT)&CGLayoutSQLTest::notify_sql_result);
   };
   ATTRIBUTE<CString sql_error>;
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

void CSQLClient::new(void) {
   new(&this->database_name).CString(NULL);

   class:base.new();
}/*CSQLClient::new*/

void CSQLClient::CSQLClient(void) {
   sqlconn_new(&this->sqlconn, SQL_Local);
}/*CSQLClient::CSQLClient*/

void CSQLClient::delete(void) {
   class:base.delete();
   
   delete(&this->database_name);   

   sqlconn_delete(this->sqlconn);   
}/*CSQLClient::delete*/

int CSQLClient::open(const char *db_filename) {
  CString(&this->database_name).set(db_filename);
  return sqlconn_open(this->sqlconn, db_filename);
}/*CSQLClient::open*/

TSQLExecRequest *CSQLClient::exec(const char *sql_statement, CObject *notify_object, SQL_NOTIFY_RESULT notify_method) {
   sqlconn_exec(this->sqlconn, sql_statement, notify_object, notify_method);
   return NULL;
}/*CSQLClient::exec*/

int CSQLClient::close(void) {
  return sqlconn_close(this->sqlconn);
}/*CSQLClient::close*/

void CSQLClientLocal::new(void) {
   ARRAY(&this->client_message).new();
   ARRAY(&this->server_message).new();
   class:base.new();
}/*CSQLClientLocal::new*/

void CSQLClientLocal::CSQLClientLocal(void) {
   sqlconn_new(&CSQLClient(this)->sqlconn, SQL_Client);
   sqlconn_new(&this->sqlconn_server, SQL_Server);   
}/*CSQLClientLocal::CSQLClientLocal*/

void CSQLClientLocal::delete(void) {
   class:base.delete();
   sqlconn_delete(this->sqlconn_server);      

   ARRAY(&this->server_message).delete();
   ARRAY(&this->client_message).delete();
}/*CSQLClientLocal::new*/

void CSQLClientLocal::write_client_message(int count, const byte *data) {
   int i;
   
   if (data) {
      for (i = 0; i < count; i++) {
         ARRAY(&this->client_message).item_add(data[i]);
      }
   }
   else {
//      printf("C: ");
//      for (i = 0; i < ARRAY(&this->client_message).count(); i++) {
//         printf("%02d ", ARRAY(&this->client_message).data()[i]);
//      }
//      printf("\n");
      
      sqlconn_server_message(this->sqlconn_server, ARRAY(&this->client_message).count(), ARRAY(&this->client_message).data(), 
                             CObject(this), (SQL_WRITE_MESSAGE)&CSQLClientLocal::write_server_message);
                            
      ARRAY(&this->client_message).used_set(0);
   }
}/*CSQLClientLocal::write_client_message*/

void CSQLClientLocal::write_server_message(int count, const byte *data) {
   int i;
   
   if (data) {
      for (i = 0; i < count; i++) {
         ARRAY(&this->server_message).item_add(data[i]);
      }
   }
   else {
//      printf("S: ");
//      for (i = 0; i < ARRAY(&this->server_message).count(); i++) {
//         printf("%02d ", ARRAY(&this->server_message).data()[i]);
//      }
//      printf("\n");
      
      sqlconn_client_message(CSQLClient(this)->sqlconn, ARRAY(&this->server_message).count(), ARRAY(&this->server_message).data(), 
                             this->notify_object, this->notify_method);

      ARRAY(&this->server_message).used_set(0);
   }
}/*CSQLClientLocal::write_server_message*/

int CSQLClientLocal::open(const char *db_filename) {
   sqlconn_message_open(CSQLClient(this)->sqlconn, db_filename, CObject(this), (SQL_WRITE_MESSAGE)&CSQLClientLocal::write_client_message);
   return 0;
}/*CSQLClientLocal::open*/

TSQLExecRequest *CSQLClientLocal::exec(const char *sql_statement, CObject *notify_object, SQL_NOTIFY_RESULT notify_method) {
   this->notify_object = notify_object;
   this->notify_method = notify_method;
   sqlconn_message_exec(CSQLClient(this)->sqlconn, sql_statement, CObject(this), (SQL_WRITE_MESSAGE)&CSQLClientLocal::write_client_message);
   
   return NULL;
}/*CSQLClientLocal::exec*/

int CSQLClientLocal::close(void) {
   sqlconn_message_close(CSQLClient(this)->sqlconn, CObject(this), (SQL_WRITE_MESSAGE)&CSQLClientLocal::write_client_message);
   return 0;   
}/*CSQLClientLocal::close*/

extern unsigned char _file_sqltest_svg[];

void CGLayoutSQLTest::notify_sql_result(int result, int rows, int columns, const char **data) {
   int i, j;
   CGGridEditColumn *column;
   CGGridEditRow *row;
   CGGridEditCell *cell;
   const char *cell_data;

   CGGridEdit(this->grid).clear();
   
   CObjPersistent(this).attribute_update(ATTRIBUTE<CGLayoutSQLTest,sql_error>);
   CString(&this->sql_error).set(result == 0 ? NULL : data[0]);
   CObjPersistent(this).attribute_update_end();   
   
   if (result != 0) {
//      CGCanvas(&this->grid->layout).queue_draw(NULL); /*>>>should not need this*/   
      CObjPersistent(this->grid).attribute_update(NULL);
      CObjPersistent(this->grid).attribute_update_end();
      return;
   }

   /* fill grid with result */
   this->grid->columns.sort_index = -1;   
   this->grid->columns.sort_direction = EGGridEditSortOrder.ascending;
   for (j = 0; j < columns; j++) {
      column = new.CGGridEditColumn();
      column->width = 120;
      column->editable = FALSE;
      CString(&column->heading).set(data[j]);
      CObject(&this->grid->columns).child_add(CObject(column));
   }
   this->grid->rows.count = rows;
   CGGridEditRows(&this->grid->rows).rebind();
   
   for (i = 0; i < rows; i++) {
      row = ARRAY(&this->grid->rows.row_index).data()[i];
      for (j = 0; j < columns; j++) {
         column = CGGridEditColumn(CObject(&this->grid->columns).child_n(j));
       
         cell = ARRAY(&row->cell).data()[j];
         
         cell_data = data[(i + 1) * columns + j];
         CString(&cell->value).set(cell_data ? cell_data : "NULL");
         
         if (CString(&cell->value).length() * 8 >= column->width) {
            column->width = CString(&cell->value).length() * 8;
         }
      }
   }
   
   CObjPersistent(this->grid).attribute_update(NULL);
   CObjPersistent(this->grid).attribute_update_end();
//   CGCanvas(&this->grid->layout).queue_draw(NULL); /*>>>should not need this*/
}/*CGLayoutSQLTest::notify_sql_result*/

void CGLayoutSQLTest::new(void) {
   new(&this->database_name).CString(NULL);
   new(&this->sql_query).CString(NULL);   
   new(&this->sql_error).CString(NULL);   

   class:base.new();
}/*CGLayoutSQLTest::new*/

unsigned char *link_me = &_file_sqltest_svg[0];

void CGLayoutSQLTest::CGLayoutSQLTest(CSQLClient *sql_client, const char *db_query, const char *db_error) {
   CObject *child;

   this->sql_client = sql_client;
   
   CString(&this->database_name).set_string(&sql_client->database_name);
   CString(&this->sql_query).set(db_query);
   CString(&this->sql_error).set(db_error);   

   new(&this->obj_server).CObjServer(CObjPersistent(this));

   CGLayout(this).CGLayout(0, 0, &this->obj_server, CObjPersistent(this));
   CGLayout(this).load_svg_file("memfile:sqltest.svg", NULL);   
   child = CObject(this).child_tree_first();
   while (child) {
      if (CObjClass_is_derived(&class(CGGridEdit), CObject(child).obj_class())) {
         this->grid = CGGridEdit(child);
      }
      child = CObject(this).child_tree_next(CObject(child));
   }
}/*CGLayoutSQLTest::CGLayoutSQLTest*/

void CGLayoutSQLTest::delete(void) {
   class:base.delete();

   delete(&this->sql_error);   
   delete(&this->sql_query);      
   delete(&this->database_name);   
   
   delete(&this->obj_server);
}/*CGLayoutSQLTest::delete*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
