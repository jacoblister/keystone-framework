/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework.c"
#include "../../src/socket.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CSQLClientApp : CApplication {
 private:
   CSocket socket;
   struct SQLConn *sql_conn;
   
   byte tx_buf[1000];
   int tx_buf_count;
   byte rx_buf[500000];
   int rx_buf_count;
   void write_message(int count, const unsigned char *data);
   void notify_result(int result, int rows, int columns, const char **data);
 public:
   void main(ARRAY<CString> *args);
};

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#include <windows.h>
bool socket_init(void) {
	WORD wVersionRequested = MAKEWORD(2, 0);
	WSADATA wsaData;
   
   return (WSAStartup(wVersionRequested, &wsaData) != SOCKET_ERROR);
}

#define PORT 		0x1234
#define DIRSIZE 	8192

OBJECT<CSQLClientApp, sqlclientapp>;

void CSQLClientApp::write_message(int count, const unsigned char *data) {
   if (count == 0) {
      CSocket(&this->socket).send(this->tx_buf, this->tx_buf_count, 0);
      this->tx_buf_count = 0;
   }
   else {
      memcpy(&this->tx_buf[this->tx_buf_count], data, count);
      this->tx_buf_count += count;
   }
}/*CSQLClientApp::write_message*/

void CSQLClientApp::notify_result(int result, int rows, int columns, const char **data) {
   printf("sql result %d, rows=%d cols=%d\n", result, rows, columns);
}/*CSQLClientApp::notify_result*/

int CSQLClientApp::main(ARRAY<CString> *args) {
   char dir[DIRSIZE];  
	int cc, fromlen, tolen;
	int addrlen;
	struct sockaddr_in sin;
	struct sockaddr_in pin;
	struct hostent *hp;	
   CSocket *current;
   int repeat = 1;

	if (!socket_init()) {
		printf("can't init");
		exit(1);
		}
      
   if (ARRAY(args).count() < 3) {
      printf("usage 'client <hostname> <sqlstatement>'\n");
      exit(1);
   }
   
   if (ARRAY(args).count() > 3) {
      repeat = atoi(CString(&ARRAY(args).data()[3]).string());
   }
   
	if ((hp = gethostbyname(CString(&ARRAY(args).data()[1]).string())) == 0) {
		perror("gethostbyname");
		exit(1);
	}		
   
	memset(&pin, 0, sizeof(sin));
	pin.sin_family = AF_INET;
	pin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
	pin.sin_port = htons(PORT);
	
   new(&this->socket).CSocket(AF_INET, SOCK_STREAM, 0);	
   
   if (CSocket(&this->socket).connect((struct sockaddr *) &pin, sizeof(pin)) == -1) {	
		perror("connect");
		exit(1);
	}
	
   sqlconn_new(&this->sql_conn, SQL_Client);

   sqlconn_message_open(this->sql_conn, "log2.db3", (void *)this, (SQL_WRITE_MESSAGE)&CSQLClientApp::write_message);
   this->rx_buf_count = CSocket(&this->socket).recv(this->rx_buf, sizeof(this->rx_buf), 0);
   sqlconn_client_message(this->sql_conn, this->rx_buf_count, this->rx_buf, (void *)this, (SQL_NOTIFY_RESULT)&CSQLClientApp::notify_result);
   
   sqlconn_message_exec(this->sql_conn, "BEGIN", (void *)this, (SQL_WRITE_MESSAGE)&CSQLClientApp::write_message);
   this->rx_buf_count = CSocket(&this->socket).recv(this->rx_buf, sizeof(this->rx_buf), 0);
   sqlconn_client_message(this->sql_conn, this->rx_buf_count, this->rx_buf, (void *)this, (SQL_NOTIFY_RESULT)&CSQLClientApp::notify_result);

	while (this->rx_buf_count > 0 && repeat) {
      sqlconn_message_exec(this->sql_conn, CString(&ARRAY(args).data()[2]).string(), (void *)this, (SQL_WRITE_MESSAGE)&CSQLClientApp::write_message);
      this->rx_buf_count = CSocket(&this->socket).recv(this->rx_buf, sizeof(this->rx_buf), 0);
      if (this->rx_buf_count > 0) {
         sqlconn_client_message(this->sql_conn, this->rx_buf_count, this->rx_buf, (void *)this, (SQL_NOTIFY_RESULT)&CSQLClientApp::notify_result);
      }
      repeat--;
   }

   sqlconn_message_exec(this->sql_conn, "END", (void *)this, (SQL_WRITE_MESSAGE)&CSQLClientApp::write_message);
   this->rx_buf_count = CSocket(&this->socket).recv(this->rx_buf, sizeof(this->rx_buf), 0);
   sqlconn_client_message(this->sql_conn, this->rx_buf_count, this->rx_buf, (void *)this, (SQL_NOTIFY_RESULT)&CSQLClientApp::notify_result);
	
   CSocket(&this->socket).close();
   
   delete(&this->socket);
   
   return 0;
}/*CSQLClientApp::main*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
