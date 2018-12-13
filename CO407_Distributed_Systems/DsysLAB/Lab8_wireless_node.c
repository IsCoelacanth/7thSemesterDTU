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

typedef struct wireless_node
{
    int priority;
    int parent;
} wireless_node;

wireless_node w;

int max(int a, int b)
{
    return a >= b? a:b;
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

void startElection(int id, int *procs, int num_procs, int self)
{
    int itr;
    char message[ML];
    sprintf(message, "%s %d", "ELEC", self);
    for (itr = 0; itr < num_procs; itr++)
    {
        if (procs[itr] != w.parent)
        {
            printf("Sending elections to: %d\n", procs[itr]);
            send_to_id(procs[itr], id, message);
        }
    }
}

void announce_completion(int self, int *procs, int num_procs, int coord)
{
    int itr;
    char message[ML];
    sprintf(message, "%s %d", "DONE", coord);

    for (itr = 0; itr < num_procs; itr++)
    {
        send_to_id(procs[itr], self, message);
    }
}

void propagate_completion(int self, int *procs, int num_procs, char M[ML])
{
    int itr;

    for (itr = 0; itr < num_procs; itr++)
    {
        send_to_id(procs[itr], self, M);
    }
}

int main(int argc, char* argv[])
{
    int self = atoi(argv[1]);
    int n_procs = atoi(argv[2]);
    int procs[MPROC];
    int sender, pcnt = 0, ecnt = 0;
    int sock_id, coord_id;
    int itr, len, n, start, ix;

    char buffer[ML], flag[ML], p_id[ML], msg[256];

    struct sockaddr_in from;

    w.priority = atoi(argv[3]);
    w.parent = -1;
    coord_id = w.priority; 
    for(itr = 0; itr < n_procs; itr += 1)
        procs[itr] = atoi(argv[4 + itr]);
    start = atoi(argv[4 + n_procs]) == 1? TRUE:FALSE;

    printf("Creating node at %d\n", self);
    sock_id = connect_to_port(self);

    if (start == TRUE)
    {
        startElection(sock_id, procs, n_procs, self);
    }

    while(TRUE)
    {
        if (start != TRUE && ecnt + 1 == n_procs)
        {
            sprintf(msg, "RTRN %d", coord_id);
            send_to_id(w.parent, \
            sock_id,
            msg);
            printf("Sending to parent %d\n", w.parent);
        }
        if (pcnt == n_procs)
        {
            if (start == TRUE)
            {
                printf("Announcing completion\n");
                announce_completion(sock_id, procs, n_procs, coord_id);
                exit(1);
            }
            else
            {
                sprintf(msg, "RTRN %d", coord_id);
                send_to_id(w.parent, \
                sock_id,
                msg);
                printf("Sending to parent %d\n", w.parent);
            }
        }
        
        memset(&from, 0, sizeof(from));
        // printf("Tring read\n");
        n = recvfrom(sock_id, (char *)buffer, ML, MSG_WAITALL, (struct sockaddr *)&from, &len);
        buffer[n] = '\0';
        printf("Recieved: %s\n", buffer);
        for(itr = 0; itr < 4; itr++)
        {
            // printf("%c %d\n", buffer[itr], itr);
            flag[itr] = buffer[itr];
        }
        flag[itr] = '\0';
        printf("Extracted flag \n");
        if (strcmp(flag, "RTRN") == 0 || strcmp(flag, "DONE") == 0)
        {
            for(ix=0, itr = itr + 1; itr < 6; itr++)
                p_id[ix++] = buffer[itr];
        }
        else
        {
            for(ix=0, itr = itr + 1; itr < 9; itr++)
                p_id[ix++] = buffer[itr];
        }
        p_id[ix] = '\0';
        sender = atoi(p_id);
        // printf("%s %d\n", flag, sender);

        if (strcmp(flag, "ELEC") == 0)
        {
            if (w.parent == -1)
            {
                w.parent = sender;
                printf("Set parent to %d\n", w.parent);
                if (n_procs == 1 && procs[0] == w.parent)
                    pcnt ++;
                startElection(sock_id, procs, n_procs, self);
            }
            else
            {
                printf("Sending EACK to %d\n", sender);
                send_to_id(sender, sock_id, "EACK 0000");
            }
        }
        else if (strcmp(flag, "EACK") == 0)
        {
            ecnt += 1;
            continue;
        }
        else if (strcmp(flag, "RTRN") == 0)
        {
            pcnt += 1;
            if (w.priority < sender)
            {
                printf("Changed potential coord to: %d\n", sender);
                coord_id = sender;
            }
            else
                coord_id = max(coord_id, w.priority);
        }
        else if (strcmp(flag, "DONE") == 0)
        {
            if (w.priority != sender)
                propagate_completion(sock_id, procs, n_procs, buffer);
            else
                printf("SET SELF AS CONTROLLER\n");
            exit(1);
        }
        // printf("Waiting\n");
    }
    return 0;
}