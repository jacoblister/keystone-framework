/*!\file
 \brief Keystone source file

 Keystone Alpha Release

 by Jacob Lister - 2004.

 contact j_lister@paradise.net.nz
*/
/*==========================================================================*/
MODULE::IMPORT/*============================================================*/
/*==========================================================================*/

#include "framework_base.c"
#include "syncobject.c"

/*==========================================================================*/
MODULE::INTERFACE/*=========================================================*/
/*==========================================================================*/

/*>>>shouldn't include platform system files here!  likely cause of
     slow preprocessor operation*/
#if OS_Win32
#include <io.h>
#include <fcntl.h>
#include <winsock2.h>
#include "win32/native.h"
#endif
#if OS_Linux
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "linux/native.h"
#endif

#if !OS_PalmOS
class CSocket : CIOObject {
 public:
   void CSocket(int domain, int type, int protocol);

   virtual void write_string(char *buffer);
   virtual void read_string(CString *string);

   int bind(struct sockaddr *my_addr, int addrlen);
   int connect(struct sockaddr *my_addr, int addrlen);
   CSocket *accept(CSocket *socket, struct sockaddr *my_addr, socklen_t *addrlen);
   int listen(int backlog);
   int send(const void *msg, size_t len, int flags);
   int sendto(const char *buf, int len, int flags, const struct sockaddr *to, int tolen);
   int recv(char *buf, int len, int flags);
   int recvfrom(char *buf, int len, int flags, struct sockaddr *from, socklen_t *fromlen);
   void close(void);
};
#endif

/*==========================================================================*/
MODULE::IMPLEMENTATION/*====================================================*/
/*==========================================================================*/

#if !OS_PalmOS
void CSocket::CSocket(int domain, int type, int protocol) {
   CIOObject(this)->handle = socket(domain, type, protocol);
}/*CSocket::CSocket*/

void CSocket::write_string(char *buffer) {
   int n;
   /*write a string to a connection oriented socket*/
   n = CSocket(this).send(buffer, strlen(buffer) + 1, 0);
   if (n != (int)(strlen(buffer) + 1)) {
      ASSERT("incomplete send");
   }
}/*CSocket::write_string*/

int CSocket::read_string(CString *string) {
   int i, n, recv_count;
   char message[2000];
   bool done = FALSE;

   CString(string).clear();

   while (!done) {
      n = CSocket(this).recv(message, sizeof(message), MSG_PEEK); /* wait for data */
      if (n <= 0) {
          return n;
      }

      for (i = 0; i < n; i++) {
         if (message[i] == '\0') {
            done = TRUE;
            break;
         }
      }
      i++;

      recv_count = 0;
      while (i > 0) {
         n = CSocket(this).recv(message + recv_count, i, 0);
         if (n <= 0) {
             return n;
         }
         i -= n;
         recv_count += n;
      }
      CString(string).appendn(message, recv_count);
   }
   return 0;
}/*CSocket::read_string*/

int CSocket::bind(struct sockaddr *my_addr, int addrlen) {
   return bind(CIOObject(this)->handle, my_addr, addrlen);
}/*CSocket::bind*/

int CSocket::connect(struct sockaddr *my_addr, int addrlen) {
   return connect(CIOObject(this)->handle, my_addr, addrlen);
}/*CSocket::connect*/

CSocket *CSocket::accept(CSocket *socket, struct sockaddr *my_addr, socklen_t *addrlen) {
   /*>>>use object copy */
   if (!socket) {
      socket = CSocket(new.class(&class(CSocket)));
   }

   memcpy(socket, this, sizeof(CSocket));

   CIOObject(socket)->handle = accept(CIOObject(this)->handle, my_addr, addrlen);
   return socket;
}/*CSocket::accept*/

int CSocket::listen(int backlog) {
   return listen(CIOObject(this)->handle, backlog);
}/*CSocket::listen*/

int CSocket::send(const void *msg, size_t len, int flags) {
   return send(CIOObject(this)->handle, msg, len, flags);
}/*CSocket::send*/

int CSocket::sendto(const char *buf, int len, int flags, const struct sockaddr *to, int tolen) {
   return sendto(CIOObject(this)->handle, buf, len, flags, to, tolen);
}/*CSocket::sendto*/

int CSocket::recv(char *buf, int len, int flags) {
   return recv(CIOObject(this)->handle, buf, len, flags);
}/*CSocket::recv*/

int CSocket::recvfrom(char *buf, int len, int flags, struct sockaddr *from, socklen_t *fromlen) {
   return recvfrom(CIOObject(this)->handle, buf, len, flags, from, fromlen);
}/*CSocket::recvfrom*/

void CSocket::close(void) {
//   closesocket(CIOObject(this)->handle);
}/*CSocket::close*/

#endif

/*==========================================================================*/
MODULE::END/*===============================================================*/
/*==========================================================================*/

