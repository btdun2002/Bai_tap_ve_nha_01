#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Invalid argument.\n");
        return 1;
    }

    int port = atoi(argv[1]);
    char *hello_file = argv[2];
    char *recv_file = argv[3];

    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5))
    {
        perror("listen() failed");
        return 1;
    }

    printf("Initialized server  \n");

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    int client = accept(listener, (struct sockaddr *)&client_addr, &client_addr_len);

    printf("Accepted socket %d from IP: %s:%d\n", client, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    FILE *greeting_file = fopen(argv[2], "r");
    if (greeting_file == NULL)
    {
        perror("fopen() failed");
        return 1;
    }
    char greetings[256];
    if (fgets(greetings, sizeof(greetings), greeting_file) == NULL)
    {
        perror("fgets() failed");
        return 1;
    }
    send(client, greetings, strlen(greetings), 0);
    fclose(greeting_file);

    FILE *output_file = fopen(argv[3], "w");
    if (output_file == NULL)
    {
        perror("fopen() failed");
        return 1;
    }
    else
    {
        printf("Opened output file\n");
    }

    int recv_bytes;
    char buf[256];
    char ack_msg[268];

    while (1)
    {
        memset(buf, 0, sizeof(buf));
        recv_bytes = recv(client, buf, sizeof(buf), 0);
        if (recv_bytes < 0)
        {
            perror("recv() failed");
            return 1;
        }
        fwrite(buf, 1, recv_bytes, output_file);
        fflush(output_file);
        if (strncmp(buf, "exit", 4) == 0)
        {
            break;
        }
        sprintf(ack_msg, "Received: %s\n", buf);
        printf("%s", ack_msg);
        send(client, ack_msg, sizeof(ack_msg), 0);
    }
    fclose(output_file);
    close(client);
    close(listener);
}