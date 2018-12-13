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
#include <string.h>
#define MSG_CONFIRM 0

#define TRUE 1
#define FALSE 0
#define ML 1024
#define MPROC 32

typedef struct Resource
{
    int a;
    int b;
    int c;
    int d;
    int e;
} Resource;

void serealize(Resource S, char output[ML])
{
    sprintf(output, "DONE %d\t%d\t%d\t%d\t%d\t", S.a, S.b, S.c, S.d, S.e);
}

Resource unserealize(char input[ML])
{
    char temp[ML];
    int ix = 0, itr = 5;
    Resource S;
    for(itr; input[itr] != '\t'; itr +=1)
    {
        printf("%d %c\n", itr, input[itr]);
        temp[ix++] = input[itr];
    }
    temp[ix] = '\0';
    S.a = atoi(temp);
    ix = 0;
    printf("here\n");
    for(itr = itr + 1; input[itr] != '\t'; itr +=1)
        temp[ix++] = input[itr];
    temp[ix] = '\0';
    S.b = atoi(temp);
    ix = 0;

    for(itr = itr + 1; input[itr] != '\t'; itr +=1)
        temp[ix++] = input[itr];
    temp[ix] = '\0';
    S.c = atoi(temp);
    ix = 0;

    for(itr = itr + 1; input[itr] != '\t'; itr +=1)
        temp[ix++] = input[itr];
    temp[ix] = '\0';
    S.d = atoi(temp);
    ix = 0;

    for(itr = itr + 1; input[itr] != '\t'; itr +=1)
        temp[ix++] = input[itr];
    temp[ix] = '\0';
    S.e = atoi(temp);
    ix = 0;
    return S;
}

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

void send_to_id(int to, int from, char message[ML])
{
    struct sockaddr_in cl;
    memset(&cl, 0, sizeof(cl));

    cl.sin_family = AF_INET;
    cl.sin_addr.s_addr = INADDR_ANY;
    cl.sin_port = htons(to);

    sendto(
        from, \
        (const char *)message, \
        strlen(message), \
        MSG_CONFIRM, \
        (const struct sockaddr *)&cl, \
        sizeof(cl));
}

void request_key(int server, int sock_id, int a)
{
    char msg[256];
    sprintf(msg, "KEYR %d", a);
    send_to_id(server, sock_id, msg);
}

int main(int argc, char* argv[])
{
    int self = atoi(argv[1]);
    int server = atoi(argv[2]);
    int start = atoi(argv[3]);
    int udelay = atoi(argv[4]);
    int itr;
    int dest;
    int key = 0;

    int sock_id, len, n;
    char buffer[ML], msg[ML];
    char flag[256], p_id[256];

    struct sockaddr_in from;

    Resource S = {0, 0, 0, 0, 0};
    
    printf("Creating node at %d\n", self);
    sock_id = connect_to_port(self);
    
    if (start)
    {
        request_key(server, sock_id, self);
    }
    else
    {
        sleep(udelay);
        request_key(server, sock_id, self);
    }

    while(TRUE)
    {
        // sleep(udelay);
        memset(&from, 0, sizeof(from));
        n = recvfrom(sock_id, (char *)buffer, ML, MSG_WAITALL, (struct sockaddr *)&from, &len);
        buffer[n] = '\0';
        printf("Recieved: %s\n", buffer);

        for(itr = 0; itr < 4; itr ++)
            flag[itr] = buffer[itr];
        flag[itr] = '\0';
        printf("Extracted flag %s\n", flag);

        // server denies key
        if (strcmp(flag, "WAIT") == 0)
        {
            sleep(udelay);
            request_key(server, sock_id, self);
        }
        // process releases key
        else if (strcmp(flag, "PASS") == 0)
        {
            printf("Key recieved\n");
            key = 1;
            sprintf(msg, "MCON %d", self);
            send_to_id(server, sock_id, msg);
        }
        // process calls for consistency
        else if (strcmp(flag, "MCON") == 0)
        {
            printf("Pulling data from server before update\n");
            S = unserealize(buffer);
            printf("Pulled file\n %5d, %5d %5d %5d %5d\n", S.a, S.b, S.c, S.d, S.e);
        }

        else if (strcmp(flag, "CNOK") == 0 && key)
        {
            printf("Entering critical Seaction\n");
            S.a++;
            S.b++;
            S.c++;
            S.d++;
            S.e++;
            printf("Exiting critical Seaction\n");
            printf("Current file\n %5d, %5d %5d %5d %5d\n", S.a, S.b, S.c, S.d, S.e);
            serealize(S, msg);
            send_to_id(server, sock_id, msg);
            exit(EXIT_SUCCESS);
        }
    }
    return 0;
}