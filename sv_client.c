#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

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

    struct student my_student;
    char buf[512];
    printf("Enter student ID: ");
    fgets(my_student.id, sizeof(my_student.id), stdin);
    my_student.id[strcspn(my_student.id, "\n")] = '\0';
    setbuf(stdin, NULL);

    printf("Enter student name: ");
    fgets(my_student.name, sizeof(my_student.name), stdin);
    my_student.name[strcspn(my_student.name, "\n")] = '\0';
    setbuf(stdin, NULL);

    printf("Enter student date of birth (DD/MM/YYYY): ");
    fgets(my_student.dob, sizeof(my_student.dob), stdin);
    my_student.dob[strcspn(my_student.dob, "\n")] = '\0';
    setbuf(stdin, NULL);

    printf("Enter student GPA: ");
    scanf("%f", &my_student.gpa);
    setbuf(stdin, NULL);

    memset(buf, 0, sizeof(buf));
    memcpy(buf, &my_student, sizeof(my_student));
    if (send(client, buf, sizeof(buf), 0) == -1)
    {
        perror("send() failed");
        return 1;
    }
    if (recv(client, buf, sizeof(buf), 0) <= 0)
    {
        perror("recv() failed");
        return 1;
    }
    printf("Received message: %s\n", buf);
    close(client);
}