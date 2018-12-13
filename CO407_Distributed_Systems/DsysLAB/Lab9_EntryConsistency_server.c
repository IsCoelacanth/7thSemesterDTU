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
    sprintf(output, "MCON %d\t%d\t%d\t%d\t%d\t", S.a, S.b, S.c, S.d, S.e);
}

Resource unserealize(char input[ML])
{
    char temp[ML];
    int ix = 0, itr = 5;
    Resource S;
    for(itr; input[itr] != '\t'; itr +=1)
        temp[ix++] = input[itr];
    temp[ix] = '\0';
    S.a = atoi(temp);
    ix = 0;

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


void make_consistent(int from, int procs[], int n_procs, Resource S)
{
    char message[ML];
    int i;
    serealize(S, message);
    for (i = 0; i < n_procs; i++)
        send_to_id(procs[i], from, message);
}

int main(int argc, char* argv[])
{
    int self = atoi(argv[1]);
    int n_procs = atoi(argv[2]);
    int itr, ix = 0;
    int procs[MPROC];
    int key_avail = 1;
    int dest;

    int sock_id, len, n;
    char buffer[ML], msg[ML];
    char flag[256], p_id[256];

    struct sockaddr_in from;

    Resource S = {0, 0, 0, 0, 0};
    
    for(itr = 0; itr < n_procs; itr ++)
        procs[itr] = atoi(argv[3 + itr]);
    
    printf("Creating node at %d\n", self);
    sock_id = connect_to_port(self);

    while(TRUE)
    {
        memset(&from, 0, sizeof(from));
        n = recvfrom(sock_id, (char *)buffer, ML, MSG_WAITALL, (struct sockaddr *)&from, &len);
        buffer[n] = '\0';
        printf("Recieved: %s\n", buffer);

        for(itr = 0; itr < 4; itr ++)
            flag[itr] = buffer[itr];
        flag[itr] = '\0';
        printf("Extracted flag %s\n", flag);

        // process asks for key
        if (strcmp(flag, "KEYR") == 0)
        {
            ix = 0;
            for (itr = 5; itr < 9; itr++)
                p_id[ix++] = buffer[itr];
            p_id[ix] = '\0';
            dest = atoi(p_id);
            printf("Extracted dest %d\n", dest);
            if (key_avail)
            {
                send_to_id(dest, sock_id, "PASS");
                key_avail = 0;
            }
            else
            {
                send_to_id(dest, sock_id, "WAIT");
            }
        }
        // process releases key
        else if (strcmp(flag, "DONE") == 0)
        {
            printf("Key released\n");
            S = unserealize(buffer);
            key_avail = 1;
        }
        // process calls for consistency
        else if (strcmp(flag, "MCON") == 0)
        {
            printf("Forcing consistency \n");
            make_consistent(sock_id, procs, n_procs, S);
            for (itr = 5; itr < 9; itr++)
                p_id[5-itr] = buffer[itr];
            p_id[5-itr] = '\0';
            dest = atoi(p_id);
            send_to_id(dest, sock_id, "CNOK");
        }
    }

}