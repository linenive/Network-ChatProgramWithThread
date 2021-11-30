#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#define BUFF_SIZE 1024
#define MAX_CLIENT_CONNECT 5

pthread_t pt[MAX_CLIENT_CONNECT];
int client_fds[MAX_CLIENT_CONNECT+1];

        struct myinfo
        {
                char name[80];
                short int age;
                int birthday;
        };

void *communicateWithClient(void *data)
{
        int client_index = *(int *)data;

        char buff[BUFF_SIZE+5];
        int byte_number;

        while(1){
                memset(buff, 0, sizeof(buff));
                byte_number = read(client_fds[client_index], buff, BUFF_SIZE);
                if(byte_number<=0) continue;
                printf("Client %d: %s\n", client_index, buff);
                if(buff[0]=='b' && buff[1]=='y' && buff[2]=='e')
                {
                        printf("Close client %d.\n", client_index);
                        write(client_fds[client_index], "exit", strlen("exit")+1);
                        continue;
                }
        }

        /*
        close(client_fds[client_index]);
        client_fds[client_index] = -1;
        pthread_exit((void *) 0);
        */
}
int isStringEqualToAll(char* text)
{
        //printf("%lu %lu\n", sizeof(text), sizeof("all")); // 8, 4
        return strlen(text)==3 && text[0]=='a'
                        && text[1]=='l' && text[2]=='l';
}

void *waitUserInput()
{
        char input_text[BUFF_SIZE];
        char *temp_text;
        int target_client_index = -1;
        int i;

        while(1)
        {
                fgets(input_text, BUFF_SIZE, stdin);
                temp_text = strtok(input_text, " ");
                if(temp_text == NULL)
                {
                        printf("WARNING: Invalid input!\n");
                        continue;
                }
                if(isStringEqualToAll(temp_text))
                {
                        target_client_index = -2;
                }
                else{
                        target_client_index = temp_text[0] - '0';
                }                
                temp_text = strtok(NULL, "\n");
                if(temp_text == NULL)
                {
                        printf("WARNING: Invalid input!\n");
                        continue;
                }
                //printf("target: %d, message: %s\n", target_client_index, temp_text);

                if(target_client_index == -2){
                        for(i=1; i<MAX_CLIENT_CONNECT+1; i++)
                        {
                                if(client_fds[i] != -1){
                                        write(client_fds[i], temp_text, strlen(temp_text)+1);
                                }
                        }
                }
                else if(0<target_client_index && target_client_index <= MAX_CLIENT_CONNECT){
                        if(client_fds[target_client_index] == -1){
                                printf("WARNING: There's no client connect! input index: %d\n",
                                target_client_index);
                        }
                        write(client_fds[target_client_index], temp_text, strlen(temp_text)+1);
                }
                else{
                       printf("WARNING: Invalid client index. The range is: %d ~ %d.\n",
                       1, MAX_CLIENT_CONNECT+1); 
                }
                
        }
}

int main(int argc, char **argv)
{
        int server_fd = 0;
        int client_fd = 0;
        int client_addr_size = 0;
        int i;

        pthread_t input_pt;

        struct sockaddr_in server_addr;
        struct sockaddr_in client_addr;

        memset(&server_addr, 0x00, sizeof(server_addr));
        memset(&client_addr, 0x00, sizeof(client_addr));
        for(i=1; i<MAX_CLIENT_CONNECT+1; i++)
        {
            client_fds[i] = -1;
        }

        server_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(-1 == server_fd)
        {
                printf("Error: Fail to create server socket\n");
                exit(1);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(4000);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if(-1 == bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)))
        {
                printf("Error: Error run bind()\n");
                exit(1);
        }

        if(-1 == listen(server_fd, 5))
        {
                printf("Error: Fail to run listen()\n");
                exit(1);
        }

        pthread_create(&input_pt, NULL, waitUserInput, NULL);
        
        while(1)
        {
                client_addr_size = sizeof(client_addr);
                client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_size);

                if (-1 == client_fd)
                {
                        printf("Error: Fail to accept client link\n");
                        exit(1);
                }

                for(i=1; i<MAX_CLIENT_CONNECT+1; i++)
                {
                        if(client_fds[i] == -1)
                        {
                                client_fds[i] = client_fd;
                                pthread_create(&pt[i], NULL, communicateWithClient, (void *)&i);
                                printf("System: Client %d is connected. fd: %d\n", i, client_fd);
                                break;
                        }
                        if(i==MAX_CLIENT_CONNECT-1){
                                printf("Error: Trying to accept too many clients. ");
                                printf("max client number: %d\n", MAX_CLIENT_CONNECT);
                        }
                }
        }
        return 0;
}
