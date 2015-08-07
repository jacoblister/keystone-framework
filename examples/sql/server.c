#include "../../src/extras/sqlconn.h"

#include <windows.h>
int socket_init(void) {
   WORD wVersionRequested = MAKEWORD(2, 0);
   WSADATA wsaData;

   return (WSAStartup(wVersionRequested, &wsaData) != SOCKET_ERROR);
}

#define PORT      0x1234
#define MSGSIZE   8192
#define MAXCONNECT 2
#define MAXTXMESSAGE 500000

typedef struct {
   int fd;
   struct SQLConn *sql_conn;
} TConnection;

typedef struct {
   byte buf[MAXTXMESSAGE];
   int buf_count;
int msg_count;   
int buf_count_last;
   TConnection con[MAXCONNECT];
   int count;

   int fd_socket;
   fd_set readfds;
   int fd_max;
} TConnections;

TConnections connections;

void CONNECTION_write_message(TConnection *cp, int count, const unsigned char *data) {
   int sent;
   if (count == 0) {
      sent = send(cp->fd, connections.buf, connections.buf_count, 0);
      printf("tx message %d bytes (%d sent)\n", connections.buf_count, sent); 
      connections.msg_count++;      
      connections.buf_count_last = connections.buf_count;
      connections.buf_count = 0;
   }
   else {
      memcpy(&connections.buf[connections.buf_count], data, count);
      connections.buf_count += count;
   }
}/*CONNECTION_write_message*/

void CONNECTIONS_new();
void CONNECTIONS_delete();
TConnection *CONNECTIONS_add(int fd);
void CONNECTIONS_remove(TConnection *connection);
TConnection *CONNECTIONS_wait(void);

void CONNECTIONS_new(fd_socket) {
   int i;
 
   connections.fd_socket = fd_socket;
   FD_ZERO(&connections.readfds);

   FD_SET(connections.fd_socket, &connections.readfds);
   connections.fd_max = fd_socket;
   
   for (i = 0; i < MAXCONNECT; i++) {
      connections.con[i].fd = -1;
   }
}/*CONNECTIONS_new*/

void CONNECTIONS_delete(void) {
}/*CONNECTIONS_delete*/

TConnection *CONNECTIONS_add(int fd) {
   int i;
   TConnection *cp;

   for (i = 0; i < MAXCONNECT; i++) {
      cp = &connections.con[i];
      if (cp->fd == 0 || cp->fd == -1) {
         break;
      }
   }
   if (i == MAXCONNECT) {
      return NULL;
   }

   cp->fd = fd;
   FD_SET(fd, &connections.readfds);   
   if (fd > connections.fd_max) {
      connections.fd_max = fd;
   }
   sqlconn_new(&cp->sql_conn, SQL_Server);
   
   return cp;
}/*CONNECTIONS_add*/

void CONNECTIONS_remove(TConnection *connection) {
   int i;
   TConnection *cp;
   
   connection->fd = -1;
   
   FD_ZERO(&connections.readfds);
   FD_SET(connections.fd_socket, &connections.readfds);      
   connections.fd_max = connections.fd_socket;
   for (i = 0; i < MAXCONNECT; i++) {
      cp = &connections.con[i];
      if (cp->fd != -1) {
         sqlconn_delete(cp->sql_conn);
         FD_SET(cp->fd, &connections.readfds);
         if (cp->fd > connections.fd_max) {
            connections.fd_max = cp->fd;
         }
      }
   }
}/*CONNECTIONS_remove*/

TConnection *CONNECTIONS_wait(void) {
   int i;
   fd_set readfds;
   struct timeval timeout;
  
   memset(&timeout, 0, sizeof(timeout));

   memcpy(&readfds, &connections.readfds, sizeof(fd_set));
   select(connections.fd_max + 1, &readfds, NULL, NULL, NULL);
   if (FD_ISSET(connections.fd_socket, &readfds)) {
      return NULL;
   }
   
   for (i = 0; i < MAXCONNECT; i++) {
      if (FD_ISSET(connections.con[i].fd, &readfds)) {
         return &connections.con[i];
      }
   }
   
   printf("select error");
   exit(1);
   return NULL;
}/*CONNECTIONS_wait*/

int main(int argc, char **argv) {
   char msg[MSGSIZE];
   int cc, fromlen, tolen, peeksize;
   int addrlen;
   struct sockaddr_in sin;
   struct sockaddr_in pin;
   TConnection *cp;
   int fd_socket, fd_current;
   int length, offset;

   if (!socket_init()) {
      printf("can't init");
      exit(1);
      }

   fd_socket = socket(AF_INET, SOCK_STREAM, 0);
//   fd_socket = socket(AF_INET, SOCK_SEQPACKET, 0);
   memset(&sin, 0, sizeof(sin));
   sin.sin_family = AF_INET;
   sin.sin_addr.s_addr = INADDR_ANY;
   sin.sin_port = htons(PORT);

   /* bind the socket to the port number */
   if (bind(fd_socket, (struct sockaddr *) &sin, sizeof(sin)) == -1) {
      printf("bind");
      exit(1);
   }

   /* show that we are willing to listen */
   if (listen(fd_socket, MAXCONNECT + 1) == -1) {
      printf("listen");
      exit(1);
   }

   CONNECTIONS_new(fd_socket);
   while (1) {
      cp = CONNECTIONS_wait();
      if (cp) {
#if 0     
         memset(msg, 0, sizeof(msg));
         cc = recv(cp->fd, msg, 4, 0);
         length = ntohl(*((u_long *)msg));
         offset = 0;
         while (offset < length) {
            cc = recv(cp->fd, msg + offset, length, 0);
            if (cc <= 0) {
               printf("Close connection %d\n", cp->fd);          
               closesocket(cp->fd);       
               CONNECTIONS_remove(cp);
               continue;
            }
            else {
               offset += cc;
            }
         }
#else         
         memset(msg, 0, sizeof(msg));
         cc = recv(cp->fd, msg, sizeof(msg), 0);
         if (cc <= 0) {
            printf("Close connection %d\n", cp->fd);          
            closesocket(cp->fd);       
            CONNECTIONS_remove(cp);
            continue;
         }
         length = cc;         
#endif         
         
         printf("rx message %d, %d bytes (%s)\n", cp->fd, length, msg);
         if (0) {
            cc = send(cp->fd, connections.buf, connections.buf_count_last, 0);
            printf("tx message (chache) %d bytes (%d sent)\n", connections.buf_count, cc); 
         }
         else {
            sqlconn_server_message(cp->sql_conn, cc, msg, cp, (SQL_WRITE_MESSAGE)CONNECTION_write_message);    
         }
      }
      else {
         addrlen = sizeof(pin);
         fd_current = accept(fd_socket, (struct sockaddr *)&pin, &addrlen);
         if (fd_current == -1) {
            printf("accept");
            exit(1);
         }
         cp = CONNECTIONS_add(fd_current);
         if (cp) {
            printf("Accept connection from %s#%d (%d)\n",inet_ntoa(pin.sin_addr),ntohs(pin.sin_port), fd_current);          
         }
         else {
            printf("Refuse connection from %s#%d (%d)\n",inet_ntoa(pin.sin_addr),ntohs(pin.sin_port), fd_current);          
            send(fd_current, "Too Many Connections", 21, 0);
            closesocket(fd_current);
         }
      }
   }
   CONNECTIONS_delete();
   
   closesocket(fd_socket);

   return 0;
}/*main*/