#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#define BUFF_SIZE 1024

        struct myinfo
        {
                char name[80];
                short int age;
                int birthday;
        };

void communicateWithServer(int client_fd)
{
        char buff[BUFF_SIZE+5];
        
        read(client_fd, buff, BUFF_SIZE);
        printf("[c_recv]%s\n", buff);
        /*
        struct myinfo info;
        while(1){
                write(client_fd, argv[1], strlen(argv[1])+1);
                read(client_fd, buff, BUFF_SIZE);
                printf("[c_recv]%s\n", buff);

                memset(&info, 0, sizeof(info));
                strcpy(info.name, "Janjani");
                info.age = htons(1);
                info.birthday = htonl(305419896);
                write(client_fd, (struct myinfo*)&info, sizeof(info));
        }
        */
        
        close(client_fd);
}

int main(int argc)
{
        int client_fd;
        struct sockaddr_in server_addr;
        

        // int domain, int type, int protocol
        // return socket descriptor
        client_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(-1 == client_fd)
        {
                printf("socket 생성 실패\n");
                exit(1);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(4000);
        server_addr.sin_addr.s_addr= inet_addr("127.0.0.1");

        if(-1 == connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)))
        {
                printf( "접속 실패\n");
                exit(1);
        }

        communicateWithServer(client_fd);

        return 0;
}
~                                                                               
~                    