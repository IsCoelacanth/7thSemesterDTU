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
#include<unistd.h>

typedef struct resources
{
    int A;
    char B;
    int C;
    char D;
}resources;

int main()
{
	struct sockaddr_in sa;  // Socket address data structure
    resources R;
	int n, sockfd; // read and source 
	char buff[1025], obuff[256]; // buffer to store the read stream
	int snded, rec;

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
	sa.sin_port = htons(8888);

	// establishing and verifying the connection
	if (connect(sockfd, (struct sockaddr_in*)&sa, sizeof(sa)) < 0)
	{
		printf("Connection failed\n");
		exit(0);
	}
	else
		printf("Connection made\n");
	
	while (1)
	{
		snded = write(sockfd, "PING", 5);
		if (snded > -1)
			printf("SENT PING\n");
		rec = read(sockfd, obuff, 256);
		obuff[rec] = '\0';
		if (strcmp(obuff, "PONG") == 0)
        {
            usleep(750);
            FILE *f;
            f = fopen("shared_mem.txt", "r");
            fread(&R, sizeof(R), 1, f);
            fclose(f);
            printf("read %d, %d, %d, %d from server\n", R.A, R.B, R.C, R.D );
            R.A += 1;
            R.B += 1;
            R.C += 1;
            R.D += 1;
            f = fopen("shared_mem.txt", "w");
            fwrite(&R, sizeof(R), 1, f);
            fclose(f);
            printf("Got access to CS\n");
			snded = write(sockfd, "DONE", 4);
            printf("Freeing Lock\n");
            break;
        }
	}
	// Reading and priting data from the server after verification
	close(sockfd); // Closing the socket
	return 0;
}
