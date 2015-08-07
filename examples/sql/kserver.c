/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework.c"
#include "../../src/socket.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

class CSQLServerApp : CApplication {
 private:
   CSocket socket;
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

OBJECT<CSQLServerApp, sqlserverapp>;

int CSQLServerApp::main(ARRAY<CString> *args) {
   char dir[DIRSIZE];  
	int cc, fromlen, tolen;
	int addrlen;
	struct sockaddr_in sin;
	struct sockaddr_in pin;
   CSocket *current;

	if (!socket_init()) {
		printf("can't init");
		exit(1);
		}

   new(&this->socket).CSocket(AF_INET, SOCK_STREAM, 0);
   
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(PORT);
   
	/* bind the socket to the port number */
	if (CSocket(&this->socket).bind((struct sockaddr *) &sin, sizeof(sin)) == -1) {
		printf("bind");
		exit(1);
	}

	/* show that we are willing to listen */
	if (CSocket(&this->socket).listen(5) == -1) {
		printf("listen");
		exit(1);
	}
	/* wait for a client to talk to us */
   addrlen = sizeof(pin); 
   current = CSocket(&this->socket).accept(&this->socket, (struct sockaddr *)&pin, &addrlen);
	if (!current) {
		printf("accept");
		exit(1);
	}
   
printf("Hi there, from  %s#\n",inet_ntoa(pin.sin_addr));
printf("Coming from port %d\n",ntohs(pin.sin_port));

   memset(dir, 0, sizeof(dir));
   cc = CSocket(current).recv(dir, sizeof(dir), 0);
	while (cc != 0 && cc != -1) {
	   printf("rx message (%s)\n", dir);
      memset(dir, 0, sizeof(dir));		
      cc = CSocket(current).recv(dir, sizeof(dir), 0);
	}
   
   delete(&this->socket);
   
   return 0;
}/*CSQLServerApp::main*/

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/
