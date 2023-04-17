#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Invalid argument.\n");
        return 1;
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(server_ip);
    addr.sin_port = htons(port);

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect() failed");
        return 1;
    }

    printf("Connected to server %s %d\n", server_ip, port);

    char buf[256];
    if (recv(client, buf, sizeof(buf), 0) <= 0)
    {
        perror("recv() failed");
        return 1;
    }
    printf("Received message: %s\n", buf);

    while (1)
    {
        memset(buf, 0, sizeof(buf));

        printf("Enter message: ");
        fgets(buf, sizeof(buf), stdin);
        if (send(client, buf, strlen(buf), 0) == -1)
        {
            perror("send() failed");
            return 1;
        }
        if (strncmp(buf, "exit", 4) == 0)
            break;

        if (recv(client, buf, sizeof(buf), 0) <= 0)
        {
            perror("recv() failed");
            return 1;
        }
        printf("Received message: %s\n", buf);
    }

    close(client);
}