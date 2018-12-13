#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

typedef struct resources
{
	int A;
	int B;
	int C;
	int D;
} resources;

int main()
{
	resources R, temp;
	R.A = 1;
	R.B = 2;
	R.C = 3;
	R.D = 4;
	FILE *fle;
	fle = fopen("shared_mem.txt", "w");
	fwrite(&R, sizeof(R), 1, fle);
	fclose(fle);
	struct sockaddr_in sa; // Socket address data structure
	int opt = TRUE, addrlen;
	int sockfd, clients[50]; // Source and destination addresses
	char buff[256];			 // Buffer to hold the out-going stream
	int rec, i, sd, activity, new_sock, sended;
	int max_sd;
	int flag = 0;
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // New socket created
											  // Checking for valid socket
	memset(clients, 0, sizeof(clients));

	fd_set readfds;
	if (sockfd < 0)
	{
		printf("Error in creating socket\n");
		exit(0);
	}
	else
	{
		printf("Socket Created\n");
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		printf("error\n");
	}
	// Clearing and assigning type and address to the socket
	printf("Socket created\n");
	bzero(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(8888);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	// binding and verifying the socket to address
	if (bind(sockfd, (struct sockaddr *)&sa, sizeof(sa)) < 0)
	{
		printf("Bind Error\n");
	}
	else
		printf("Binded\n");

	// starts the server with a max client queue size set as 10
	listen(sockfd, 10);
	addrlen = sizeof(sa);
	// server run
	while (TRUE)
	{
		// Clearing socket set
		FD_ZERO(&readfds);

		FD_SET(sockfd, &readfds);
		max_sd = sockfd;
		for (i = 0; i < 50; i++)
		{
			sd = clients[i];
			if (sd > 0)
				FD_SET(sd, &readfds);
			if (sd > max_sd)
				max_sd = sd;
		}
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
		if (activity < 0)
			printf("Select error\n");
		if (FD_ISSET(sockfd, &readfds))
		{
			if ((new_sock = accept(sockfd, (struct sockaddr *)NULL, NULL)) < 0)
				perror("accept");
			else
			{
				printf("New connection, sock fd %d\n", new_sock);
			}
			sended = send(new_sock, buff, strlen(buff), 0);
			if (sended < 0)
				perror("Send");
			for (i = 0; i < 50; i++)
			{
				if (clients[i] == 0)
				{
					clients[i] = new_sock;
					break;
				}
			}
		}
		for (i = 0; i < 50; i++)
		{
			sd = clients[i];
			if (FD_ISSET(sd, &readfds))
			{
				FILE *fle;
				fle = fopen("shared_mem.txt", "r");
				fread(&temp, sizeof(temp), 1, fle);
				fclose(fle);
				rec = read(sd, buff, 256);
				if (rec == 0)
				{
					getpeername(sd, (struct sockaddr *)&sa,
								(socklen_t *)&sa);
					printf("%d has disconnected unexpectedly with ip %s and port %d\n", sd, inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
					printf("recovering data\n");
					FILE *fle;
					fle = fopen("shared_mem.txt", "w+");
					fwrite(&temp, sizeof(temp), 1, fle);
					fclose(fle);
					close(sd);
					clients[i] = 0;
				}
				else
				{
					buff[rec] = '\0';
					printf("recieved %s from %d\n", buff, sd);

					if (strcmp(buff, "PING") == 0 && flag == 1)
					{
						printf("Read buffer = %s, from %d and send NACK\n", buff, sd);
						sended = write(sd, "NACK", 4);
					}

					else if (strcmp(buff, "PING") == 0 && flag == 0)
					{
						printf("Read Buffer = %s, from %d\n", buff, sd);
						flag = 1;
						sended = write(sd, "PONG", 4);
						// rec = read(sd, buff, 4);
						// if (rec > 0 && strcmp(buff, "DONE") == 0)
						// {
						//     printf("Lock freed\n");
						//     flag = 0;
						//     FILE *fle;
						//     fle = fopen("shared_mem.txt", "r");
						//     fread(&temp, sizeof(temp), 1, fle);
						//     printf("Read %d, %d, %d, %d from %d\n", temp.A, temp.B, temp.C, temp.D, sd );
						//     fclose(fle);
						//     clients[i] = 0;
						//     close(sd);
						//     break;
						// }
					}
					else if (strcmp(buff, "DONE") == 0)
					{
						printf("Lock freed\n");
						flag = 0;
						FILE *fle;
						fle = fopen("shared_mem.txt", "r");
						fread(&temp, sizeof(temp), 1, fle);
						printf("Read %d, %d, %d, %d from %d\n", temp.A, temp.B, temp.C, temp.D, sd);
						fclose(fle);
						clients[i] = 0;
						close(sd);
						break;
					}
				}
			}
		}
	}
	close(sockfd); // close the socket
	return 0;
}
