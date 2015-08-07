/*-------------------------- SQL Connection ----------------------------------
IN	  [PL]SQLCONN.C
By	  Jacob Lister

Copyright Abbey Systems 1997..2009

2009-09-12	TN	Add standard history block, reformat code

NOTE:

---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include "sqlconn.h"

#define UNICODE_NULL "\342\220\200"

#ifndef NULL
#define NULL 0
#endif

#define LOWORD(w) (unsigned char)((w) & 0xFF)
#define HIWORD(w) (unsigned char)(((w) >> 8) & 0xFF)
#define MKWORD(h,l) ((short)(h) << 8 | (short)(l))

typedef enum {
   mc_open,
   mc_exec,
   mc_close,
} ESQLCommMessageClient;

#if OPT_NULL

typedef struct SQLConn {
	ESQLConnectType type;
	} TSQLConn;

void sqlconn_new(struct SQLConn **sqlconn, ESQLConnectType type) { *sqlconn=NULL;}
void sqlconn_delete(struct SQLConn *sqlconn) {};
int sqlconn_open(struct SQLConn *sqlconn, const char *db_filename) {return  -1;}
void sqlconn_exec(struct SQLConn *sqlconn, const char *sql_statement, void *notify_object, SQL_NOTIFY_RESULT notify_result) {}
int sqlconn_close(struct SQLConn *sqlconn) {return -1; }
void sqlconn_message_open(struct SQLConn *sqlconn, const char *db_filename, void *write_object, SQL_WRITE_MESSAGE write_message) {}
void sqlconn_message_exec(struct SQLConn *sqlconn, const char *sql_statement, void *write_object, SQL_WRITE_MESSAGE write_message) {}
void sqlconn_message_close(struct SQLConn *sqlconn, void *write_object, SQL_WRITE_MESSAGE write_message) {}
void sqlconn_server_message(struct SQLConn *sqlconn, int count, unsigned char *data, void *write_object, SQL_WRITE_MESSAGE write_message) {}
void sqlconn_client_message(struct SQLConn *sqlconn, int count, unsigned char *data, void *notify_object, SQL_NOTIFY_RESULT notify_result) {}

#else //Not OPT_NULL

#include "sqlite3.h"

typedef struct SQLConn {
	ESQLConnectType type;
	sqlite3 *database;
	} TSQLConn;


/*! \brief initialize a new connection
 *  \param[out] sqlconn returns a pointer to new database connection
 *  \param[in] type type of database connection required
 *  \post \code *sqlconn != NULL; \endcode
 */
void sqlconn_new(struct SQLConn **sqlconn, ESQLConnectType type) {
	*sqlconn = calloc(sizeof(TSQLConn), 1);
	(*sqlconn)->type = type;
	}// sqlconn_new


/*! \brief release a connection */
void sqlconn_delete(struct SQLConn *sqlconn) {
	free(sqlconn);
	}// sqlconn_delete


/*! \brief open a local database connection
 *  \pre \code sqlconn->type == SQL_Local \endcode
 *  \param[in] sqlconn pointer to database connection
 *  \param[in] db_filename database filename
 *  \retval result
 *    - 0 - Success
 *    - nonzero - failure
 */
int sqlconn_open(struct SQLConn *sqlconn, const char *db_filename) {
	return sqlite3_open(db_filename, &sqlconn->database);
	}// sqlconn_open


/*! \brief execute a local query
 *  \pre \code sqlconn->type == SQL_Local \endcode
 *  \pre \code sql_statement != NULL \endcode
 *  \param[in] sqlconn pointer to database connection
 *  \param[in] sql_statement sql statement or series of statements to execute
 *  \param[in] notify_object application defined callback pointer
 *  \param[in] notify_result application defined callback function to receive result
 */
void sqlconn_exec(struct SQLConn *sqlconn, const char *sql_statement, void *notify_object, SQL_NOTIFY_RESULT notify_result) {
	char **result_data, *result_error;
	int result;
	int result_rows, result_cols;
	result = sqlite3_get_table(sqlconn->database, sql_statement, &result_data, &result_rows, &result_cols, &result_error);
	if (result == 0) {
		notify_result(notify_object, result, result_rows, result_cols, (const char **)result_data);
		}
	else {
		notify_result(notify_object, result, result_rows, result_cols, (const char **)&result_error);
		}
	sqlite3_free_table(result_data);
	}// sqlconn_exec


/*! \brief close a local database connection
 *  \pre \code sqlconn->type == SQL_Local \endcode
 *  \param[in] sqlconn pointer to database connection
 *  \retval result
 *    - 0 - Success
 *    - nonzero - failure
 */
int sqlconn_close(struct SQLConn *sqlconn) {
	return sqlite3_close(sqlconn->database);
	}// sqlconn_close


/*! \brief construct SQL open message
 *  \pre \code sqlconn->type == SQL_Client \endcode
 *  \param[in] sqlconn pointer to database connection
 *  \param[in] db_filename database filename or identifier
 *  \param[in] write_object application defined callback pointer
 *  \param[in] write_message application defined callback function to receive message
 *
 * <table border="1" bordercolour="lightgrey" frame="box" rules="all" cellspacing="0">
 * <tr align="center"><td>Msg<br>Type</td><td colspan="10">Database Name</td></tr>
 * <tr align="center"><td>2</td>
 *    <td>'e'</td><td>'v'</td><td>'e'</td><td>'n'</td><td>'t'</td><td>'.'</td><td>'d'</td><td>'b'</td><td>'3'</td><td>0</td>
 * </tr>
 * </table>
 *
 * If 'db_filename' is NULL, the filename string is omitted in the message
 *
 * <table border="1" bordercolour="lightgrey" frame="box" rules="all" cellspacing="0">
 * <tr align="center"><td>Msg<br>Type</td></tr>
 * <tr align="center"><td>2</td></tr>
 * </table>
 */
void sqlconn_message_open(struct SQLConn *sqlconn, const char *db_filename, void *write_object, SQL_WRITE_MESSAGE write_message) {
	unsigned char type = mc_open;
	sqlconn = sqlconn; /*shh*/
	write_message(write_object, 1, &type);
	if (db_filename) {
		write_message(write_object, strlen(db_filename) + 1, (const unsigned char *)db_filename);
		}
	write_message(write_object, 0, NULL);
	}// sqlconn_message_open


/*! \brief construct SQL exec message
 *  \pre \code sqlconn->type == SQL_Client \endcode
 *  \pre \code sql_statement != NULL \endcode
 *  \param[in] sqlconn pointer to database connection
 *  \param[in] sql_statement sql statement or series of statements to execute
 *  \param[in] write_object application defined callback pointer
 *  \param[in] write_message application defined callback function to receive message
 *
 * <table border="1" bordercolour="lightgrey" frame="box" rules="all" cellspacing="0">
 * <tr align="center"><td>Msg<br>Type</td><td colspan="2">Msg ID</td><td>Response<br>Form</td><td colspan="19">Query Text</td></tr>
 * <tr align="center"><td>1</td><td>0</td><td>0</td><td>0</td>
 *    <td>'S'</td><td>'E'</td><td>'L'</td><td>'E'</td><td>'C'</td><td>'T'</td>
 *    <td>'*'</td><td>'F'</td><td>'R'</td><td>'O'</td><td>'M'</td>
 *    <td>' '</td><td>'p'</td><td>'e'</td><td>'o'</td><td>'p'</td><td>'l'</td><td>'e'</td><td>0</td>
 * </tr>
 * </table>
 *
 * Parameters within the message
 * - Msg ID 16 bit value identifing message, copied to result by server on execution
 * - ResponseForm requests in what encoding provided result should be formatted.
 *   At present, only '0' is defined (ASCII), but other options may be provided in
 *   future
 *   - 0 ASCII
 *   - 1 Binary (native data types)
 *   - 128 Compressed ASCII
 */
void sqlconn_message_exec(struct SQLConn *sqlconn, const char *sql_statement, void *write_object, SQL_WRITE_MESSAGE write_message) {
	unsigned char type = mc_exec;
	short id = 0;
	unsigned char form = 0;
	sqlconn = sqlconn; /*shh*/
	write_message(write_object, 1, &type);
	write_message(write_object, 2, (unsigned char *)&id);
	write_message(write_object, 1, &form);
	write_message(write_object, strlen(sql_statement) + 1, (const unsigned char *)sql_statement);
	write_message(write_object, 0, NULL);
	}// sqlconn_message_exec


/*! \brief construct SQL close message
 *  \pre \code sqlconn->type == SQL_Client \endcode
 *  \param[in] sqlconn pointer to database connection
 *  \param[in] write_object application defined callback pointer
 *  \param[in] write_message application defined callback function to receive message
 *
 * <table border="1" bordercolour="lightgrey" frame="box" rules="all" cellspacing="0">
 * <tr align="center"><td>Msg<br>Type</td></tr>
 * <tr align="center"><td>2</td></tr>
 * </table>
 */
void sqlconn_message_close(struct SQLConn *sqlconn, void *write_object, SQL_WRITE_MESSAGE write_message) {
	unsigned char type = mc_close;
	sqlconn = sqlconn; /*shh*/
	write_message(write_object, 1, &type);
	write_message(write_object, 0, NULL);
	}// sqlconn_message_close


/*! \brief handle client message and execute application callback with result
 *  \pre \code sqlconn->type == SQL_Client; \endcode
 *  \param[in] sqlconn pointer to database connection
 *  \param[in] count number of bytes in message
 *  \param[in] data message data
 *  \param[in] notify_object application defined callback pointer
 *  \param[in] notify_result message application defined callback function to receive message
 */
void sqlconn_client_message(struct SQLConn *sqlconn, int count, const unsigned char *data, void *notify_object, SQL_NOTIFY_RESULT notify_result) {
	/*>>>handle misformed messages, handle message number, handle byte ordering*/
	const char **table_data;
	int rows, cols;
	int i;
	const char *cp;
	count = count; /*shh*/
	sqlconn = sqlconn; /*shh*/
	switch (data[0]) {
	case mc_open:
		break;
	case mc_exec:
		if (data[3] == 0) {
			rows = MKWORD(data[5], data[6]);
			cols = MKWORD(data[7], data[8]);
			table_data = malloc(sizeof(char *) * (rows + 1) * cols);
			cp = (const char *)&data[9];
			i = 0;
			while (i < (rows + 1) * cols) {
				table_data[i] = memcmp(cp, UNICODE_NULL, sizeof(UNICODE_NULL)) == 0 ? NULL : cp;
				cp = strchr(cp, '\0') + 1;
				i++;
				}//while
			notify_result(notify_object, 0, rows, cols, table_data);
			free((void *)table_data);
			}//if
		else {
			table_data = malloc(sizeof(char *));
			table_data[0] = (const char *)&data[4];
			notify_result(notify_object, 0xFF, 0, 0, table_data);
			free((void *)table_data);
			}
		}//switch
	}// sqlconn_client_message


/*! \brief execute SQL request message at server, as generated by SQL client message functions
 *  \pre \code sqlconn->type == SQL_Server \endcode
 *  \param[in] sqlconn pointer to database connection
 *  \param[in] count number of bytes in data
 *  \param[in] data message data
 *  \param[in] write_object application defined callback pointer
 *  \param[in] write_message application defined callback function to receive message
 *
 * Fields in the message have the following meaning:
 * - Msg Type, Server/Client response is always the same as message type sent to the server
 *   - 0 - Open
 *   - 1 - Exec
 *   - 2 - Close
 * - Msg ID - 16 bit message ID copied from client request
 * - Result - 8 bit result code
 *   - 0 - OK
 *   - Other values - Error
 *
 * The following responses may be given:
 * - Msg Type = 0 (Open)
 *   - Response
 *     <table border="1" bordercolour="lightgrey" frame="box" rules="all" cellspacing="0">
 *     <tr align="center"><td>Msg<br>Type</td><td>Result</td></tr>
 *     <tr align="center"><td>0</td><td>0</td></tr>
 *     </table>
 *
 * - Msg Type = 1 (Exec)
 *   - Response Form 0<br>
 *     Returns an array of 0 terminated strings representing the query result.<br>
 *     For fields where the value is NULL, the sequence 0xE2,0x90,0x80 is returned.<br>
 *     This is UTF-8 encoding for the character U+2400, 'NUL' in Unicode.<br><br>
 *     <table border="1" bordercolour="lightgrey" frame="box" rules="all" cellspacing="0">
 *     <tr align="center"><td>Msg<br>Type</td><td colspan="2">Msg ID</td><td>Result</td><td>Response<br>Form</td><td colspan="2">Rows</td><td colspan="2">Columns</td></tr>
 *     <tr align="center"><td>1</td><td>0</td><td>0</td><td>0</td><td>0</td><td>0</td><td>3</td><td>0</td><td>2</td></tr>
 *     </table><br>
 *     <table border="1" bordercolour="lightgrey" frame="box" rules="all" cellspacing="0">
 *     <tr align="center"><td colspan="5">Heading<br>Col 0</td><td colspan="4">Heading<br>Col 1</td>
 *     <tr align="center"><td>'N'</td><td>'a'</td><td>'m'</td><td>'e'</td><td>0</td><td>'A'</td><td>'g'</td><td>'e'</td><td>0</td>
 *     </table><br>
 *     <table border="1" bordercolour="lightgrey" frame="box" rules="all" cellspacing="0">
 *     <tr align="center"><td colspan="6">Row 0<br>Col 0</th><td colspan="3">Row 0<br>Col 1</td>
 *     <tr align="center"><td>'A'</td><td>'l'</td><td>'i'</td><td>'c'</td><td>'e'</td><td>0</td><td>'4'</td><td>'3'</td><td>0</td>
 *     </table><br>
 *     <table border="1" bordercolour="lightgrey" frame="box" rules="all" cellspacing="0">
 *     <tr align="center"><td colspan="4">Row 1<br>Col 0</td><td colspan="3">Row 1<br>Col 1</td>
 *     <tr align="center"><td>'B'</td><td>'o'</td><td>'b'</td><td>0</td><td>'2'</td><td>'8'</td><td>0</td>
 *     </table><br>
 *     <table border="1" bordercolour="lightgrey" frame="box" rules="all" cellspacing="0">
 *     <tr align="center"><td colspan="6">Row 2<br>Col 0</td><td colspan="4">Row 2<br>Col 1</td>
 *     <tr align="center"><td>'C'</td><td>'i'</td><td>'n'</td><td>'d'</td><td>'y'</td><td>0</td><td>0xE2</td><td>0x90</td><td>0x80</td><td>0</td>
 *     </table><br>
 *   - Error Response
 *     <table border="1" bordercolour="lightgrey" frame="box" rules="all" cellspacing="0">
 *     <tr align="center"><td>Msg<br>Type</td><td colspan="2">Msg ID</td><td>Result</td><td colspan="22">Error Message</td>
 *     <tr align="center"><td>1</td><td>0</td><td>0</td><td>0xFF</td>
 *     <td>'n'</td><td>'o'</td><td>' '</td><td>'s'</td><td>'u'</td><td>'c'</td><td>'h'</td><td>' '</td>
 *     <td>'t'</td><td>'a'</td><td>'b'</td><td>'l'</td><td>'e'</td><td>':'</td><td>' '</td><td>'p'</td><td>'e'</td><td>'o'</td><td>'p'</td><td>'l'</td><td>'e'</td><td>0</td>
 *     </table><br>
 */
void sqlconn_server_message(struct SQLConn *sqlconn, int count, const unsigned char *data, void *write_object, SQL_WRITE_MESSAGE write_message) {
	char **result_data, *result_error;
	unsigned char message_type;
	unsigned char result;
	int result_rows, result_cols;
	int i;
	unsigned char write_data[2];
	count = count; /*shh*/
	/*>>>handle misformed messages, handle byte ordering*/
	switch (data[0]) {
	case mc_open:
		result = (unsigned char)sqlite3_open((const char *)&data[1], &sqlconn->database);
		message_type = mc_open;
		write_message(write_object, 1, &message_type);
		write_message(write_object, 1, &result);
		write_message(write_object, 0, NULL);
		break;
	case mc_exec:
		/*>>> use SQLite row step API for better performance */
		if (!sqlconn->database) {
			result = (unsigned char)-1;
			result_data = 0;
			result_rows = 0;
			result_cols = 0;
			result_error = "no open database";
			}
		else {
			result = (unsigned char)sqlite3_get_table(sqlconn->database, (const char *)&data[4], &result_data, &result_rows, &result_cols, &result_error);
			}
		if (result == 0) {
			message_type = mc_exec;
			write_message(write_object, 1, &message_type);
			write_data[0] = data[1];
			write_data[1] = data[2];
			write_message(write_object, 2, write_data); /* message id */
			write_data[0] = result;
			write_message(write_object, 1, write_data);
			write_message(write_object, 1, &data[3]); /* response form */
			write_data[0] = HIWORD(result_rows);
			write_data[1] = LOWORD(result_rows);
			write_message(write_object, 2, write_data);
			write_data[0] = HIWORD(result_cols);
			write_data[1] = LOWORD(result_cols);
			write_message(write_object, 2, write_data);
			for (i = 0; i < (result_rows + 1) * result_cols; i++) {
				if (result_data[i]) {
					write_message(write_object, strlen(result_data[i]) + 1, (const unsigned char *)result_data[i]);
					}
				else {
					write_message(write_object, sizeof(UNICODE_NULL), (const unsigned char *)UNICODE_NULL);
					}
				}//for
			}//if result == 0
		else {
			message_type = mc_exec;
			write_message(write_object, 1, &message_type);
			write_data[0] = data[1];
			write_data[1] = data[2];
			write_message(write_object, 2, write_data); /* message id */
			write_data[0] = result;
			write_message(write_object, 1, write_data);
			if (result_error) {
				write_message(write_object, strlen(result_error) + 1, (const unsigned char *)result_error);
				}
			else {
				write_message(write_object, 2, (const unsigned char *)"?");
				}
			}//else result != 0
		write_message(write_object, 0, NULL);
		sqlite3_free_table(result_data);
		break;
	case mc_close:
		result = (unsigned char)sqlite3_close(sqlconn->database);
		sqlconn->database = NULL;
		message_type = mc_close;
		write_message(write_object, 1, &message_type);
		write_message(write_object, 1, &result);
		write_message(write_object, 0, NULL);
		break;
		}//switch data[0]
	}// sqlconn_server_message

#endif //Not OPT_NULL