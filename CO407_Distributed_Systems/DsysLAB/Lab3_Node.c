#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
 
#define MAXLINE 1024

int create_connection(int PORT)
{
    int sockfd;
	struct sockaddr_in servaddr;
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	int optval = 1;
  	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family    = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
    return sockfd;
}

void critical_section(int MY_PORT)
{
	printf("Entering the critical section\n");
	sleep(5);
	FILE *fp;
	fp = fopen("common_resource.txt", "a");
	fprintf(fp, "Written by process %d\n", MY_PORT);
	fclose(fp);
}
 
int main(int argc, char* argv[]) {

	int MY_PORT= atoi(argv[1]);
	int NEXT_CLIENT_PORT= atoi(argv[2]);
	int IS_INITIATOR= atoi(argv[3]);
	
    char buffer[MAXLINE];
	
	printf("Initialising the server at port %d.\n", MY_PORT);
    int sockfd = create_connection(MY_PORT);

    struct sockaddr_in next_client_addr, prev_client_addr;
    int len, n;

    char response[MAXLINE];
	memset(&next_client_addr, 0, sizeof(next_client_addr));
	next_client_addr.sin_family    = AF_INET; // IPv4
	next_client_addr.sin_addr.s_addr = INADDR_ANY;
	next_client_addr.sin_port = htons(NEXT_CLIENT_PORT);

    if(IS_INITIATOR)
    {
		critical_section(MY_PORT);
		strcpy(response, "ACK");
		int check = sendto(sockfd, (const char *)response, strlen(response), MSG_CONFIRM, (const struct sockaddr *) &next_client_addr, sizeof(next_client_addr));
		memset(&prev_client_addr, 0, sizeof(prev_client_addr));
		n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &prev_client_addr, &len);
		buffer[n] = '\0';
		if(!strcmp(buffer, "ACK"))
		{
			strcpy(response, "TERM");
			sendto(sockfd, (const char *)response, strlen(response), MSG_CONFIRM, (const struct sockaddr *) &next_client_addr, sizeof(next_client_addr));
			printf("Exiting\n");
		}
		else
		{
			printf("Invalid message recieved\n");

		}
		exit(0);
    }
    else
    {
    	while(1)
    	{
	    	memset(&prev_client_addr, 0, sizeof(prev_client_addr));
			n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &prev_client_addr, &len);
			buffer[n] = '\0';
			if(!strcmp(buffer, "ACK"))
			{
				critical_section(MY_PORT);
				sendto(sockfd, (const char *)buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *) &next_client_addr, sizeof(next_client_addr));
			}
			else if(!strcmp(buffer, "TERM"))
			{
				sendto(sockfd, (const char *)buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *) &next_client_addr, sizeof(next_client_addr));
				printf("Exiting\n");
				exit(0);
			}
			else
			{
				printf("Invalid message recieved\n");
			}
		}
    }
}