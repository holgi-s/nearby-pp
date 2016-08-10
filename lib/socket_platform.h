//
// Created by Holger on 17.05.2016.
//

#ifndef NEARBY_SOCK_HELP_H
#define NEARBY_SOCK_HELP_H

#ifdef __WIN32__
# include <winsock2.h>
# include <inaddr.h>
# ifndef socklen_t
#  define socklen_t int
# endif
#else
# include <sys/socket.h>
# include <sys/select.h>
# include <netinet/in.h>
# include <unistd.h>
# include <signal.h>
# ifndef SOCKET
#  define SOCKET int
#  define SOCKET_ERROR -1
#  define INVALID_SOCKET -1
# endif
#  define closesocket close
#endif


#endif //NEARBY_SOCK_HELP_H
