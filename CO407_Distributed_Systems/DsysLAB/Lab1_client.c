#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

int main()
{
	struct sockaddr_in sa;  // Socket address data structure

	int n, sockfd; // read and source 
	char buff[1025]; // buffer to store the read stream


	sockfd = socket(PF_INET, SOCK_STREAM, 0); // New socket created


    // Checking for valid socket
	if (sockfd < 0)
	{
		printf("Error in creation\n");
		exit(0);
	}
	else
		printf("Socket created\n");

    // Clearing and assigning type and address to the socket
	bzero(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(5600);
	
    // establishing and verifying the connection
	if (connect(sockfd, (struct sockaddr_in*)&sa, sizeof(sa)) < 0)
	{
		printf("Connection failed\n");
		exit(0);
	}
	else
		printf("Connection made\n");
	
    // Reading and priting data from the server after verification
	if ( n = read(sockfd, buff, sizeof(buff)) < 0)
	{
		printf("Read Error\n");
		exit(0);
	}
	else
	{
        printf("Read message: %s\n", buff);
		printf("%s\n", buff);
        printf("Done with connection, exiting\n");
	}
	close(sockfd); // Closing the socket
	return 0;
}
