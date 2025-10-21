/* tcp_client.c
 *
 *  ANSI‑C TCP client – source file
 *
 *  Author:  <your‑name>
 *  Date:    <today>
 *
 *  Compile with:
 *      gcc -o tcp_client tcp_client.c
 *
 *  Includes the header file that declares everything.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>          /* close() */
#include "tcp.h"

/*--- global instance -------------------------------------*/
struct tcp_client client;

/*--- create a socket & initialise the address structure --*/
void tcp_client_init(void)
{
    /* create a TCP socket */
    client.sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client.sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* clear the address structure */
    memset(&client.addr, 0, sizeof(client.addr));

    /* set the address family to IPv4 */
    client.addr.sin_family = AF_INET;

    /* set the port (network byte order) */
    client.addr.sin_port = htons(SERVER_PORT);

    /* set the IP address (network byte order) */
    client.addr.sin_addr.s_addr = inet_addr(SERVER_IP);
}

/*--- connect the socket to the server --------------------*/
void tcp_client_connect(void)
{
    if (connect(client.sockfd,
                (struct sockaddr *)&client.addr,
                sizeof(client.addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
}

/*--- send a message to the server ------------------------*/
void tcp_client_send(const char *msg, size_t len)
{
    ssize_t sent = send(client.sockfd, msg, len, 0);
    if (sent < 0) {
        perror("send");
        exit(EXIT_FAILURE);
    }
}

/*--- receive a message from the server -------------------*/
void tcp_client_recv(char *buf, size_t len)
{
    ssize_t recvd = recv(client.sockfd, buf, len, 0);
    if (recvd < 0) {
        perror("recv");
        exit(EXIT_FAILURE);
    }
}

/*--- close the socket ------------------------------------*/
void tcp_client_close(void)
{
    close(client.sockfd);
}
/* main.c – example program that uses the client */

#include <stdio.h>
#include "tcp_client.h"

int main(void)
{
    /* initialise, connect, send, receive, close */
    tcp_client_init();
    tcp_client_connect();

    const char *msg = "Hello, world!";
    tcp_client_send(msg, strlen(msg));

    char reply[256];
    tcp_client_recv(reply, sizeof(reply));
    printf("Received: %s\n", reply);

    tcp_client_close();
    return 0;
}
