#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#define BUFF_SIZE 1024
#define MAX_CLIENT_CONNET 5

pthread_t pt[MAX_CLIENT_CONNECT];
int client_fds[MAX_CLIENT_CONNECT];

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
        char buff_rcv[BUFF_SIZE];
        char buff_snd[BUFF_SIZE];
        /*
        struct myinfo info;
        while(1){
                read(client_fd, buff_rcv, BUFF_SIZE);
                printf("[s_recv]receive: %s\n", buff_rcv);

                sprintf(buff_snd, "%zu : %s", strlen(buff_rcv), buff_rcv);
                write(client_fd, buff_snd, strlen(buff_snd)+1);

                memset(&info, 0, sizeof(info));
                read(client_fd, (struct myinfo *)&info, BUFF_SIZE);
                printf("[s_recv]receive name: %s, age: %d, birthday: %d\n", info.name, ntohs(info.age), ntohl(info.birthday));
                
        }
        */
        close(client_fds[client_index]);
        client_fds[client_index] = -1;
        pthread_exit((void *) 0);
}

void *waitUserInput()
{
        char input_text[BUFF_SIZE];
        char *temp_text;
        int target_client_index = -1;

        while(1)
        {
                printf("입력: ");
                fgets(input_text, BUFF_SIZE, stdin);
                printf("%s\n", input_text);
                temp_text = strtok(input_text, " ");
                if(temp_text == NULL)
                {
                        printf("[s]WARNING: Invalid input!\n");
                        continue;
                }
                target_client_index = temp_text[0] - '0';
                if(target_client_index < 1)
                {
                        printf("[s]WARNING: Client index starts from 1, not 0.\n");
                        continue;
                }
                temp_text = strtok(NULL, " ");
                if(temp_text == NULL)
                {
                        printf("[s]WARNING: Invalid input!\n");
                        continue;
                }
                printf("target: %d, message: %s\n", target_client_index, temp_text);
                write(client_fds[target_client_index-1], "test", strlen("test")+1);
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
        for(i=0; i<MAX_CLIENT_CONNECT; i++)
        {
            client_fds[i] = -1;
        }

        server_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(-1 == server_fd)
        {
                printf("[s]Fail to create server socket\n");
                exit(1);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(4000);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if(-1 == bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)))
        {
                printf("[s]Error run bind()\n");
                exit(1);
        }

        if(-1 == listen(server_fd, 5))
        {
                printf("[s]Fail to run listen()\n");
                exit(1);
        }

        pthread_create(&input_pt, NULL, waitUserInput, NULL);
        
        while(1)
        {
                client_addr_size = sizeof(client_addr);
                client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_size);

                if (-1 == client_fd)
                {
                        printf("[s]Fail to accept client link\n");
                        exit(1);
                }

                for(i=0; i<MAX_CLIENT_CONNECT; i++)
                {
                        if(client_fds[i] == -1)
                        {
                           client_fds[i] = client_fd;
                                pthread_create(&pt[i], NULL, communicateWithClient, (void *)&i);
                                break;
                        }
                        if(i==MAX_CLIENT_CONNET-1)
                                printf("[s]Trying to accept too many clients. max client number: %d\n", MAX_CLIENT_CONNECT);
                }
                        
        }
        return 0;
}
