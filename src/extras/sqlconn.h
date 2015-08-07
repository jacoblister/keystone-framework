#ifndef I_SQLCONN
#define I_SQLCONN
/*!\file
 * \brief SQL Database interface
 *
 * Provides access to an SQL database.  There are two distinct modes of operation:
 * - Local - provides a local database connection whereby requests are executed and 
 *   and results provides immediately.
 * - Client/Server - provides a translation of SQL requests at the client to a 
 *   message format suitable for sending to the server.  Server responds with message 
 *   format containing the SQL response
 * 
 * When operating in Client/Server mode, SQLConn does not provide a network connection between
 * client and server, this is left to the responsibility of the application using SQLConn.  SQLConn 
 * simply generates and executes the messages needed at both ends.  
 *
 * Presently, there is only one database backend implemented for SQLConn, which is SQLite.
 * It would be possible to add other database backends, if the SQLConn API was extended to configure this.
 *
 * All strings transmitted over the protocol (with one exception see \ref sqlconn_server_message) are ASCII encoded and 0
 * terminated.  Future expansion will allow UTF-8 Unicode encoded strings to be transmitted.
 * All other data types are encoded Big Endian (MSB first) where appropriate
*/

/*! \brief Notify result of SQL query 
 *  \param[in] object application defined pointer passed to the 'exec' call 
 *  \param[in] result result code for query 
 *             - 0 Success
 *             - Any other value means error
 *  \param[in] rows number of rows in result
 *  \param[in] columns number of columns in result
 *  \param[in] data query result data.  An array of values with size ((rows + 1) * columns).
 *             The first row of the result contains the names for the columns.  NULL fields
 *             are represented by NULL pointers For example
 *             The result
 * \code 
 * Name        | Age
 * -----------------------
 * Alice       | 43
 * Bob         | 28
 * Cindy       | NULL
 * \endcode
 *             would be represented
 * \code
 * rows = 3, columns = 2;
 * data[0] = "Name";
 * data[1] = "Age";
 * data[2] = "Alice";
 * data[3] = "43";
 * data[4] = "Bob";
 * data[5] = "28";
 * data[6] = "Cindy";
 * data[7] = NULL;
 * \endcode
 *             If 'result' is any value other than 0 (success) data[0] contains error message text,
 *             for example
 * \code 
 * data[0] = "no such table: people";
 * \endcode
 */
typedef void (*SQL_NOTIFY_RESULT)(void *object, int result, int rows, int columns, const char **data);

/*! \brief Write part of a message representing an SQL operation
 *  \param[in] object application defined pointer passed to the 'message' call 
 *  \param[in] count number of bytes in message part
 *  \param[in] data message data
 *
 *  The function may be called a number of times to construct the entire message.  After
 *  the last call, the values 'count=0, data=NULL' will be passed to indicate the end 
 *  of the message
 */
typedef void (*SQL_WRITE_MESSAGE)(void *object, int count, const unsigned char *data);

typedef enum {
   SQL_Local,
   SQL_Client,
   SQL_Server,
} ESQLConnectType;

//typedef enum {
//   SQLMSG_open = 0,
//   SQLMSG_exec = 1,
//   SQLMSG_close = 2,
//} ESQLConnectMessage;

struct SQLConn;

void  sqlconn_new(struct SQLConn **sqlconn, ESQLConnectType type);
void 	sqlconn_delete(struct SQLConn *sqlconn);
int 	sqlconn_open(struct SQLConn *sqlconn, const char *db_filename);
void 	sqlconn_exec(struct SQLConn *sqlconn, const char *sql_statement, void *notify_object, SQL_NOTIFY_RESULT notify_result);
int 	sqlconn_close(struct SQLConn *sqlconn);
void 	sqlconn_message_open(struct SQLConn *sqlconn, const char *db_filename, void *write_object, SQL_WRITE_MESSAGE write_message);
void 	sqlconn_message_exec(struct SQLConn *sqlconn, const char *sql_statement, void *write_object, SQL_WRITE_MESSAGE write_message);
void 	sqlconn_message_close(struct SQLConn *sqlconn, void *write_object, SQL_WRITE_MESSAGE write_message);
void 	sqlconn_server_message(struct SQLConn *sqlconn, int count, const unsigned char *data, void *write_object, SQL_WRITE_MESSAGE write_message);
void 	sqlconn_client_message(struct SQLConn *sqlconn, int count, const unsigned char *data, void *notify_object, SQL_NOTIFY_RESULT notify_result);
#endif