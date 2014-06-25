#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <dirent.h>
#include <fnmatch.h>
#include "util.h"

#define MAXPENDING 	1    /* Max connection requests */
#define BUFFSIZE 	1024
#define TAG_BEGIN  	"SANDBOX_BEGIN"
#define TAG_ITEM    "SANDBOX_ITEM:"
#define TAG_END     "SANDBOX_END"
#define TAG_ENABLE  "SANDBOX_ENABLE:"
#define SERVERPORT   5000

static const char *s_whitelist_path = "/data/appfence/sandbox.list";

int config_sync()
{
    int serversock, clientsock;
    int port = SERVERPORT;
    struct sockaddr_in server_addr, client_addr;
    enum {
        recv_begin, recv_item, recv_end, recv_tag_enable
    } recv_stat;
    FILE *fcfg;

    /* Create the TCP socket */
    if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("Failed to create socket\n");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_addr.sin_port = htons(port);

    /* Bind the server socket */
    if (bind(serversock, (struct sockaddr *) &server_addr, sizeof(server_addr))
            < 0) {
        printf("Failed to bind the server socket\n");
        return -2;
    }
    /* Listen on the server socket */
    if (listen(serversock, MAXPENDING) < 0) {
        printf("Failed to listen on server socket\n");
        return -3;
    }

    char buffer[BUFFSIZE];
    int received = -1;
    for (;;) {
        unsigned int clientlen = sizeof(client_addr);
        /* Wait for client connection */
        if ((clientsock = accept(serversock, (struct sockaddr *) &client_addr,
                &clientlen)) < 0) {
            printf("Failed to accept client connection\n");
            continue;
        }
        printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

        recv_stat = recv_begin;
        for (;;) {
            /* Receive message */
            if ((received = recv(clientsock, buffer, BUFFSIZE - 1, 0)) < 0) {
                // printf("Disconnect to receive bytes from client\n");
                break;
            }
            buffer[received] = '\0';

            printf("Received item:(stat=%d, size=%d, %s)\n", recv_stat,
                    received, buffer);
            switch (recv_stat) {
            case recv_begin:
                if (strncmp(buffer, TAG_ENABLE, strlen(TAG_ENABLE)) == 0) {
                    char *s = buffer + strlen(TAG_ENABLE);
                    set_sandbox_enabled(atoi(s));
                } else if (strncmp(buffer, TAG_BEGIN, strlen(TAG_BEGIN)) == 0) {
                    fcfg = fopen(s_whitelist_path, "w+t");
                    recv_stat = recv_item;
                } else {
                    continue;
                }
                break;
            case recv_item:
                if (strncmp(buffer, TAG_ITEM, strlen(TAG_ITEM)) == 0) {
                    char *s = buffer + strlen(TAG_ITEM);
                    fputs(s, fcfg);
                } else if (strncmp(buffer, TAG_END, strlen(TAG_END)) == 0) {
                    fclose(fcfg);
                    kill_all_process_in_whitelist(s_whitelist_path);
                    recv_stat = recv_end;
                }
                break;
            case recv_end:
                break;
            }

            /* Send back received data */
            if (send(clientsock, "OK", 2, 0) == -1) {
                // printf("Client disconnect\n");
                break;
            }
        }
        close(clientsock);
    }

    return 0;
}
