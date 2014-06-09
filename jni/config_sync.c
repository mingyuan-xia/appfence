#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define MAXPENDING 	1    /* Max connection requests */
#define BUFFSIZE 	1024
#define TAG_BEGIN  	"SANDBOX_BEGIN"
#define TAG_ITEM    "SANDBOX_ITEM:"
#define TAG_END  	"SANDBOX_END"

int config_sync()
{
	int serversock, clientsock;
	int port;
	struct sockaddr_in server_addr, client_addr;
	enum {recv_begin, recv_item, recv_end} recv_stat;
	FILE *fcfg;

	/* Create the TCP socket */
	if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("Failed to create socket");
		return -1;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	server_addr.sin_port = htons(port);

	/* Bind the server socket */
	if (bind(serversock, (struct sockaddr *) &server_addr, sizeof(server_addr))
			< 0) {
		printf("Failed to bind the server socket");
		return -2;
	}
	/* Listen on the server socket */
	if (listen(serversock, MAXPENDING) < 0) {
		printf("Failed to listen on server socket");
		return -3;
	}

	char buffer[BUFFSIZE];
	int received = -1;
	for (;;) {
		unsigned int clientlen = sizeof(client_addr);
		/* Wait for client connection */
		if ((clientsock = accept(serversock, (struct sockaddr *) &client_addr,
				&clientlen)) < 0) {
			printf("Failed to accept client connection");
			break;
		}
		printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

		fcfg = fopen("/data/appfence/sandbox.cfg", "rw");
		recv_stat = recv_begin;
		for (;;) {
			/* Receive message */
			if ((received = recv(clientsock, buffer, BUFFSIZE - 1, 0)) < 0) {
				printf("Failed to receive bytes from client");
				break;
			}
			buffer[received] = '\0';
			switch (recv_stat) {
			case recv_begin:
				if (strncmp(buffer, TAG_BEGIN, strlen(TAG_BEGIN)) != 0) {
					send(clientsock, "ERROR", 5, 0);
					continue;
				}
				recv_stat = recv_item;
				break;
			case recv_item:
				if (strncmp(buffer, TAG_ITEM, strlen(TAG_ITEM)) == 0) {
					fputs(buffer + strlen(TAG_ITEM), fcfg);
				}
				else if (strncmp(buffer, TAG_END, strlen(TAG_END)) == 0) {
					recv_stat = recv_end;
				}
				break;
			case recv_end:
				break;
			}

			/* Send back received data */
			if (send(clientsock, "OK", 2, 0) != received) {
				printf("Failed to send bytes to client");
				break;
			}
		}
		fclose(fcfg);
		close(clientsock);
	}

	return 0;
}
