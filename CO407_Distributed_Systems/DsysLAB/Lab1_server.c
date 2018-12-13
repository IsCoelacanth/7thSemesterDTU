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

int main()
{
	struct sockaddr_in sa; // Socket address data structure

	int sockfd, coontfd; // Source and destination addresses
	char str[1025]; // Buffer to hold the out-going stream
	time_t tick; // System time data structure

	sockfd = socket(AF_INET, SOCK_STREAM, 0); // New socket created
	
    // Checking for valid socket
	if (sockfd < 0)
	{
		printf("Error in creating socket\n");
		exit(0);
	}
	else
	{
		printf("Socket Created\n");
	}

    // Clearing and assigning type and address to the socket
	printf("Socket created\n");
	bzero(&sa, sizeof(sa));
    memset(str, '0', sizeof(str)); // clearing the buffer 
	sa.sin_family = AF_INET;
	sa.sin_port = htons(5600);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

    // binding and verifying the socket to address
	if (bind(sockfd, (struct sockaddr*)&sa, sizeof(sa))<0)
	{
		printf("Bind Error\n");
	}
	else
		printf("Binded\n");


    // starts the server with a max client queue size set as 10
	listen(sockfd, 10);

    // server run
	while(1)
	{
		coontfd = accept(sockfd, (struct sockaddr*)NULL ,NULL); // Accept a request from client
		printf("Accepted\n");
        tick = time(NULL);
        snprintf(str, sizeof(str), "%.24s\r\n", ctime(&tick)); // read sys time and write to buffer
        printf("sent\n"); 
        printf("%s\n", str);
		write(coontfd, str, strlen(str)); // send buffer to client
	}

	close(sockfd); // close the socket
	return 0;
}


