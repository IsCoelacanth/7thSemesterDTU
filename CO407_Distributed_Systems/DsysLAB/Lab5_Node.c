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
#define MSG_CONFIRM 0
#define TRUE 1
#define FALSE 0
#define ML 1024
#define MPROC 32


char * get_slice(char str[], int s, int e)
{
    char ans[ML];
    int i;
    for (i = s; i < e && i <strlen(str); i++)
    {
        printf("Read %c", str[s+i]);
        ans[i] = str[s+i];
    }
    ans[i] = '\0';
    return ans;
}

char * get_max( char str[ML], int n)
{
    char *temp;
    temp = strndup(str, 4);
    printf("%s\n", temp);
    char *t2;
    int i;
    for(i = 4; i<n; i += 4)
    {
        if (i > n)
            break;
        t2 = strndup(str+i, 4);
        printf("%s\n", t2);
        if (strcmp(t2, temp)){

            temp = t2;
            printf("big %s\n", temp);
        }
    }
    temp[4] = '\0';
    printf("max: %s\n", temp);
    return temp;
}

void append_message(char str[], int id)
{
    char temp[6];
    sprintf(temp, "%d ", id);
    strcat(str, temp);
}
/*
	Function to create a new connection to port 'connect_to'
	1. Creates the socket.
	2. Binds to port.
	3. Returns socket id
*/

int connect_to_port(int connect_to)
{
	int sock_id;
	int opt = 1;
	struct sockaddr_in server;
	if ((sock_id = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("unable to create a socket");
		exit(EXIT_FAILURE);
	}
	setsockopt(sock_id, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(int));
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(connect_to);

	if (bind(sock_id, (const struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("unable to bind to port");
		exit(EXIT_FAILURE);
	}
	return sock_id;	
}

/*
	sends a message to port id to
*/

void send_to_id(int to, int id, char message[ML])
{
	struct sockaddr_in cl;
	memset(&cl, 0, sizeof(cl));

	cl.sin_family = AF_INET;
	cl.sin_addr.s_addr = INADDR_ANY;
	cl.sin_port = htons(to);

	int n = sendto(id, \
		  (const char *)message, \
		   strlen(message), \
		   MSG_CONFIRM, \
		   (const struct sockaddr *)&cl, \
		   sizeof(cl));
    printf("sent packet %d\n", n);
}

/*
	starts the election, returns 1 if it wins the round
*/

void election(int id, int to, int self)
{
    char message[ML];
    sprintf(message, "%d", self);
    printf("sending election to: %d\n", to);
    send_to_id(to, id, message);
}

/*
	announces completion by sending coord messages
*/

void announce_completion(int id, int *procs, int num_procs, int self)
{
	int itr;
	char message[ML];
	strcpy(message, "COORDINATOR");

	for (itr = 0; itr < num_procs; itr += 1)
		if (procs[itr] != self)
			send_to_id(procs[itr], id, message);
}


int main(int argc, char* argv[])
{
	// 0. Initialize variables
    int self = atoi(argv[1]);
    char *sself = argv[1];
    char *coord = "9999";
	int next = atoi(argv[2]);
	int sock_id, win_id;
	int itr, len, n, start_at;
	char buff[ML], message[ML], *id;
	struct sockaddr_in from;
	
	start_at = atoi(argv[3]);


	// 1. Create socket 
	printf("creating a node at %d %d \n", self, start_at);
	sock_id = connect_to_port(self);
	// getchar();
	// 2. check is process is initiator

	if (start_at == TRUE)
	{
		election(sock_id, next, self);
	}

	// 3. if not the initiator wait for someone else

	while(TRUE)
	{
		memset(&from, 0, sizeof(from));
		n = recvfrom(sock_id, (char *)buff, ML, MSG_WAITALL, (struct sockaddr *)&from, &len);

		buff[n] = '\0';
        

		printf("Recieved message: %s %d\n", buff, strlen(buff));
        if (buff[0] == '0')
        {
            id = strndup(buff+1, 4);
            printf("%s\n", id);
            if (strcmp(sself, id) == 0)
            {
                printf("SET SELF = COORDINATOR\n");
                strcpy(message, "COORDINATOR");
                send_to_id(next, sock_id, message);
                exit(EXIT_SUCCESS);
            }
            else
            {
                send_to_id(next, sock_id, buff);
            }
        }
        else
        {
            id = strndup(buff, 4);
            printf("Init by: %s\n", id);
            if (strcmp(id, sself) == 0)
            {
                printf("Election polling complete\n");
                coord = get_max(buff, strlen(buff));
                sprintf(message, "0%s", coord);
                // printf("%s\n", coord);
                send_to_id(next, sock_id, message);
            }
            else if(strcmp(buff, "COORDINATOR") == 0)
            {
                printf("GOT COORD ACK, EXITING ELECTION\n");
                send_to_id(next, sock_id, buff);
                exit(EXIT_SUCCESS);
            }
            else
            {
                printf("Sending to next: %d, %s\n", next, buff);
                strcat(buff, sself);
                // printf("message: %s\n", buff);
                send_to_id(next, sock_id, buff);
            }
        }
    }
	
}


