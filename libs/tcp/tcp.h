/* tcp_client.h
 *
 *  Usage example (in a separate main.c):
 *      #include "tcp_client.h"
 *
 *      int main(void)
 *      {
 *          tcp_client_init();
 *          tcp_client_connect();
 *          tcp_client_send("Hello, world!", 13);
 *          char buf[256];
 *          tcp_client_recv(buf, sizeof(buf));
 *          printf("Received: %s\n", buf);
 *          tcp_client_close();
 *          return 0;
 *      }
 */

#ifndef __TCP_H__
#define __TCP_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT 12345

struct tcp_client
{
    int                 sockfd;
    struct sockaddr_in  addr;
};

extern struct tcp_client client;

void tcp_client_init(void);

void tcp_client_connect(void);
void tcp_client_send(const char *msg, size_t len); 
void tcp_client_recv(char *buf, size_t len);

void tcp_client_close(void);

#endif 
