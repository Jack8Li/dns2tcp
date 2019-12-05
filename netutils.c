#define _GNU_SOURCE
#include "netutils.h"
#include "logutils.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#undef _GNU_SOURCE

/* AF_INET or AF_INET6 or -1(invalid ip string) */
int get_ipstr_family(const char *ipstr) {
    if (!ipstr) return -1;
    uint8_t ipaddr[16]; /* save ipv4/ipv6 addr */
    if (inet_pton(AF_INET, ipstr, &ipaddr) == 1) {
        return AF_INET;
    } else if (inet_pton(AF_INET6, ipstr, &ipaddr) == 1) {
        return AF_INET6;
    } else {
        return -1;
    }
}

/* before calling, please memset(skaddr) to 0 */
void build_skaddr4_from_ipport(skaddr4_t *skaddr, const char *ipstr, portno_t portno) {
    skaddr->sin_family = AF_INET;
    inet_pton(AF_INET, ipstr, &skaddr->sin_addr);
    skaddr->sin_port = htons(portno);
}
void build_skaddr6_from_ipport(skaddr6_t *skaddr, const char *ipstr, portno_t portno) {
    skaddr->sin6_family = AF_INET6;
    inet_pton(AF_INET6, ipstr, &skaddr->sin6_addr);
    skaddr->sin6_port = htons(portno);
}

void parse_ipport_from_skaddr4(const skaddr4_t *skaddr, char *ipstr, portno_t *portno) {
    inet_ntop(AF_INET, &skaddr->sin_addr, ipstr, IP4STRLEN);
    *portno = ntohs(skaddr->sin_port);
}
void parse_ippport_from_skaddr6(const skaddr6_t *skaddr, char *ipstr, portno_t *portno) {
    inet_ntop(AF_INET6, &skaddr->sin6_addr, ipstr, IP6STRLEN);
    *portno = ntohs(skaddr->sin6_port);
}

void make_socket_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        LOGERR("[make_socket_nonblocking] fcntl(%d, F_GETFL): (%d) %s", sockfd, errno, strerror(errno));
        exit(errno);
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        LOGERR("[make_socket_nonblocking] fctnl(%d, F_SETFL): (%d) %s", sockfd, errno, strerror(errno));
        exit(errno);
    }
}

void set_socketopt_ipv6_only(int sockfd) {
    if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &(int){1}, sizeof(int)) == -1) {
        LOGERR("[set_socketopt_ipv6_only] setsockopt(%d, IPV6_V6ONLY): (%d) %s", sockfd, errno, strerror(errno));
        exit(errno);
    }
}

void set_socketopt_reuse_addr(int sockfd) {
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1) {
        LOGERR("[set_socketopt_reuse_addr] setsockopt(%d, SO_REUSEADDR): (%d) %s", sockfd, errno, strerror(errno));
        exit(errno);
    }
}

void set_socketopt_tcp_nodelay(int sockfd) {
    if (setsockopt(sockfd, SOL_TCP, TCP_NODELAY, &(int){1}, sizeof(int)) == -1) {
        LOGERR("[set_socketopt_tcp_nodelay] setsockopt(%d, TCP_NODELAY): (%d) %s", sockfd, errno, strerror(errno));
        exit(errno);
    }
}
