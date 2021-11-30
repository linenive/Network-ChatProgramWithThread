#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#define BUFF_SIZE 1024

int client_fd;

        struct myinfo
        {
                char name[80];
                short int age;
                int birthday;
        };

int isStringEqualToBye(char* text)
{
        return  text[0]=='b' && text[1]=='y' && text[2]=='e';
}

int isStringEqualToExit(char* text)
{
        return text[0]=='e' && text[1]=='x' && text[2]=='i' && text[3]=='t';
}

void communicateWithServer()
{
        char buff[BUFF_SIZE+5];
        int byte_number;
        
        while(1){
                memset(buff, 0, sizeof(buff));
                byte_number = read(client_fd, buff, BUFF_SIZE);
                if(byte_number<=0) continue;
                if(isStringEqualToExit(buff)) break;
                printf("Server: %s\n", buff);
                if(isStringEqualToBye(buff)) 
                {
                        write(client_fd, "exit", strlen("exit")+1);
                        break;
                }
        }
}

void *waitUserInput()
{
        char input_text[BUFF_SIZE];
        char* text;

        while(1)
        {
                fgets(input_text, BUFF_SIZE, stdin);
                text = strtok(input_text, "\n");
                write(client_fd, text, strlen(text)+1);

                if(strlen(text)==3 && isStringEqualToBye(text))
                {
                        return NULL;
                }
        }        
}

int main(int argc)
{
        struct sockaddr_in server_addr;
        char buff[BUFF_SIZE+5];
        int byte_number;
        
        pthread_t input_pt;

        client_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(-1 == client_fd)
        {
                printf("Error: Fail to create socket\n");
                exit(1);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(4000);
        server_addr.sin_addr.s_addr= inet_addr("127.0.0.1");

        if(-1 == connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)))
        {
                printf( "Error: Fail to connect to server.\n");
                exit(1);
        }

        memset(buff, 0, sizeof(buff));
        byte_number = read(client_fd, buff, BUFF_SIZE);
        if(byte_number>0 && isStringEqualToExit(buff)){
                printf("Error: The server is congested. Please try again later.\n");
                exit(1);
        }else{
                printf("System: Successfully connected. ID: %s\n", buff);
        }

        pthread_create(&input_pt, NULL, waitUserInput, NULL);

        communicateWithServer();

        pthread_cancel(input_pt);
        close(client_fd);
        printf("Exit the program.\n");

        return 0;
}
