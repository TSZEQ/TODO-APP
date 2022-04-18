
#include "compat.h"

#include <lua.h>
#include <lauxlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include "timeout.h"
#include "buffer.h"

#define _VERSION "0.0.1"

#define TCPSOCK_TYPENAME     "TCPSOCKET*"
#define UDPSOCK_TYPENAME     "UDPSOCKET*"

/* Socket address */
typedef union {
  struct sockaddr sa;
  struct sockaddr_in in;
  struct sockaddr_un un;
} sockaddr_t;

/* Convert "sockaddr_t" to "struct sockaddr *". */
#define SAS2SA(x) (&((x)->sa))

/* Socket Object */
struct sockobj {
    int fd;
    int sock_family;
    double sock_timeout;        /* in seconds */
    struct buffer *buf;         /* used for buffer reading */
};

#define getsockobj(L) ((struct sockobj *)lua_touserdata(L, 1));
#define CHECK_ERRNO(expected)   (errno == expected)

/* Custom socket error strings */
#define ERROR_TIMEOUT   "Operation timed out"
#define ERROR_CLOSED    "Connection closed"
#define ERROR_REFUSED   "Connection refused"

/* Options */
#define OPT_TCP_NODELAY   "tcp_nodelay"
#define OPT_TCP_KEEPALIVE "tcp_keepalive"
#define OPT_TCP_REUSEADDR "tcp_reuseaddr"

#define RECV_BUFSIZE 8192

/**
 * Function to perform the setting of socket blocking mode.
 */
static void
__setblocking(int fd, int block)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (block) {
        flags &= (~O_NONBLOCK);
    } else {
        flags |= O_NONBLOCK;
    }
    fcntl(fd, F_SETFL, flags);
}

/**
 * Do a event polling on the socket, if necessary (sock_timeout > 0).
 *
 * Returns:
 *  1   on timeout
 *  -1  on error
 *  0   success
 */
#define EVENT_NONE      0
#define EVENT_READABLE  POLLIN
#define EVENT_WRITABLE  POLLOUT
#define EVENT_ANY       (POLLIN | POLLOUT)
static int
__waitfd(struct sockobj *s, int event, struct timeout *tm)
{
    int ret;

    // Nothing to do if socket is closed.
    if (s->fd < 0)
        return 0;

    struct pollfd pollfd;
    pollfd.fd = s->fd;
    pollfd.events = event;

    do {
        // Handling this condition here simplifies the loops.
        double left = timeout_left(tm);
        if (left == 0.0)
            return 1;
        int timeout = (int)(left * 1e3);
        ret = poll(&pollfd, 1, timeout >= 0 ? timeout : -1);
    } while (ret == -1 && CHECK_ERRNO(EINTR));

    if (ret < 0) {
        return -1;
    } else if (ret == 0) {
        return 1;
    } else {
        return 0;
    }
}

int
__select(int nfds, fd_set * readfds, fd_set * writefds, fd_set * errorfds,
         struct timeout *tm)
{
    int ret;
    do {
        struct timeval tv = { 0, 0 };
        double t = timeout_left(tm);
        if (t >= 0) {
            tv.tv_sec = (int)t;
            tv.tv_usec = (int)((t - tv.tv_sec) * 1.0e6);
        }

        ret = select(nfds, readfds, writefds, errorfds, (t >= 0) ? &tv : NULL);
    } while (ret < 0 && errno == EINTR);
    return ret;
}

/**
 * Get the address length according to the socket object's address family.
 * Return 1 if the family is known, 0 otherwise. The length is returned through