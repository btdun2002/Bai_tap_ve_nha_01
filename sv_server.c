#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

struct student
{
    char id[10];
    char name[50];
    char dob[20];
    float gpa;
};

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Invalid argument.\n");
        return 1;
    }
    int port = atoi(argv[1]);
    char *log_name = argv[2];

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

    printf("Initialized server.\n");

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    int client = accept(listener, (struct sockaddr *)&client_addr, &client_addr_len);

    printf("Accepted socket %d from IP: %s:%d\n", client, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    FILE *log_f = fopen(log_name, "a");
    if (log_f == NULL)
    {
        perror("fopen() failed");
        return 1;
    }
    else
    {
        printf("Opened log file successfully.\n");
    }
    char buf[512];
    struct student my_student;
    int recv_bytes;
    char ack_msg[256];
    recv_bytes = recv(client, buf, sizeof(buf), 0);
    if (recv_bytes == -1)
    {
        perror("recv() failed");
        return 1;
    }
    else
    {
        sprintf(ack_msg, "Received: %d bytes", recv_bytes);
        send(client, ack_msg, sizeof(ack_msg), 0);
    }
    memcpy(&my_student, buf, sizeof(my_student));
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(log_f, "%s %4d-%2d-%2d %2d:%2d:%2d %s %s %s %.2f\n",
            inet_ntoa(client_addr.sin_addr),
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec,
            my_student.id, my_student.name, my_student.dob, my_student.gpa);
    printf("%s %s %s %.2f\n", my_student.id, my_student.name, my_student.dob, my_student.gpa);
    fclose(log_f);
    close(client);
    close(listener);
}