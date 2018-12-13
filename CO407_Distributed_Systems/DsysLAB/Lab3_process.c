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

void CriticalSection()
{
    resources R;
    FILE *f;
    f = fopen("shared_mem.txt", "r");
    fread(&R, sizeof(R), 1, f);
    fclose(f);
    printf("Read %d, %d, %d, %d, from memory\n", R.A, R.B, R.C, R.D);
    printf("Working on data\n");
    R.A += 1;
    R.B += 1;
    R.C += 1;
    R.D += 1;
    f = fopen("shared_mem.txt", "w");
    fwrite(&R, sizeof(R), 1, f);
    fclose(f);
}


int Connect(int P)
{
    int sockid;
    int op_val;
    struct sockaddr_in serv_add;
    if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket Failed\n");
        exit(EXIT_FAILURE);
    }

    setsockopt(sockid, SOL_SOCKET, SO_REUSEADDR, (const void *)&op_val, sizeof(int));
    memset(&serv_add, 0, sizeof(serv_add));

    serv_add.sin_family = AF_INET;
    serv_add.sin_addr.s_addr = INADDR_ANY;
    serv_add.sin_port = htons(P);

    if (bind(sockid, (const struct sockaddr *)&serv_add, sizeof(serv_add)) < 0)
    {
        perror("Bind Error");
        exit(EXIT_FAILURE);
    }
    return sockid;
}



int main(int argc, char const *argv[])
{
    int Add, Dest, Own;
    Add = atoi(argv[1]);
    Dest = atoi(argv[2]);
    Own = atoi(argv[3]);
    printf("My address : %d Next Node 2:  %d Permission 3: %d\n",Add, Dest, Own );

    printf("Making a node at my address = %d\n", Add);
    int sock_id = Connect(Add);

    struct sockaddr_in next_node, prev_node;
    int len, n;

    char resp[1024];
    char buff[1024];

    memset(&next_node, 0, sizeof(next_node));
    next_node.sin_family = AF_INET;
    next_node.sin_addr.s_addr = INADDR_ANY;
    next_node.sin_port = htons(Dest);

    if (Own)
    {
        printf("Entering Critical Section\n");
        CriticalSection();
        strcpy(resp, "ACK");
        int c = sendto(sock_id, (const char *)resp, strlen(resp), MSG_CONFIRM,
                      (const struct sockaddr *) &next_node, sizeof(next_node));
        memset(&prev_node, 0, sizeof(prev_node));
        int n = recvfrom(sock_id, (char *)buff, 1024, MSG_WAITALL, (struct sockaddr *) &prev_node, &len);
        buff[n] = '\0';

        if (strcmp(buff, "ACK"))
        {
            strcpy(resp, "TERM");
            int c = sendto(sock_id, (const char *)resp, strlen(resp), MSG_CONFIRM,
                            (const struct sockaddr *) &next_node, sizeof(next_node));
            printf("sent to %d DONE, process exit\n", c);
        }
        else
        {
            printf("Error message\n");
        }
        exit(0);
    }

    else
    {
        while(1)
        {
            memset(&prev_node, 0, sizeof(prev_node));
            int n = recvfrom(sock_id, (char *)buff, 1024, MSG_WAITALL, ( struct sockaddr *) &prev_node, &len);
            buff[n] = '\0';
            if (!(strcmp(buff, "ACK")))
            {
                CriticalSection();
                sendto(sock_id, (const char *)buff, strlen(buff), MSG_CONFIRM, (const struct sockaddr *) &next_node, sizeof(next_node));
            }
            else if (!(strcmp(buff, "TERM")))
            {
                sendto(sock_id, (const char *)buff, strlen(buff), MSG_CONFIRM, (const struct sockaddr *) &next_node, sizeof(next_node));
                printf("Exit\n");
                exit(0);
            }
            else
            {
                printf("Invalid message\n");
            }
        }
    }


    return 0;
}



// int main()
// {
//     struct sockaddr_in sa;  // Socket address data structure
//     resources R;
//     int n, sockfd; // read and source 
//     char buff[1025], obuff[256]; // buffer to store the read stream
//     int snded, rec;

//     sockfd = socket(PF_INET, SOCK_STREAM, 0); // New socket created


//                                               // Checking for valid socket
//     if (sockfd < 0)
//     {
//         printf("Error in creation\n");
//         exit(0);
//     }
//     else
//         printf("Socket created\n");

//     // Clearing and assigning type and address to the socket
//     bzero(&sa, sizeof(sa));
//     sa.sin_family = AF_INET;
//     sa.sin_port = htons(8888);

//     // establishing and verifying the connection
//     if (connect(sockfd, (struct sockaddr_in*)&sa, sizeof(sa)) < 0)
//     {
//         printf("Connection failed\n");
//         exit(0);
//     }
//     else
//         printf("Connection made\n");
    
//     while (1)
//     {
//         snded = write(sockfd, "PING", 5);
//         if (snded > -1)
//             printf("SENT PING\n");
//         rec = read(sockfd, obuff, 256);
//         obuff[rec] = '\0';
//         if (strcmp(obuff, "PONG") == 0)
//         {
//             usleep(750);
//             FILE *f;
//             f = fopen("shared_mem.txt", "r");
//             fread(&R, sizeof(R), 1, f);
//             fclose(f);
//             printf("read %d, %d, %d, %d from server\n", R.A, R.B, R.C, R.D );
//             R.A += 1;
//             R.B += 1;
//             R.C += 1;
//             R.D += 1;
//             f = fopen("shared_mem.txt", "w");
//             fwrite(&R, sizeof(R), 1, f);
//             fclose(f);
//             printf("Got access to CS\n");
//             snded = write(sockfd, "DONE", 4);
//             printf("Freeing Lock\n");
//             break;
//         }
//     }
//     // Reading and priting data from the server after verification
//     close(sockfd); // Closing the socket
//     return 0;
// }
