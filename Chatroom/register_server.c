/*
    C socket server example
*/
 
#include<stdio.h>
#include <stdlib.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
 
int main(int argc , char *argv[])
{ 
    int socket_desc , client_sock , c , read_size, pid;
    struct sockaddr_in server , client;
    char client_message[2000], message_sent[2000], message_recieve[2000];
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 5555 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    while (1) {
        //Accept and incoming connection
        puts("Waiting for incoming connections...");
        c = sizeof(struct sockaddr_in);
     
        // accept connection from an incoming client
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0)
        {
            perror("accept failed");
            return 1;
        }

        /* Create child process */
        pid = fork();
      if (pid == 0) {
         /* This is the client process */
        close(socket_desc);
        puts("Connection accepted");
	
        char *username, *password;
        memset(message_recieve, 0, sizeof message_recieve);
        puts("uff");
        recv(client_sock , message_recieve , 2000 , 0);

        //printf("New user registered : %s", username);
        puts("jag");
        //puts(message_recieve);
        username = strtok(message_recieve, " ");
        password = strtok(NULL, " ");
        //puts(message_recieve);
    



        FILE *file = fopen("registeredUsers.txt", "r");
        if (file == NULL)
        {
            printf("Error opening file!\n");
            exit(1);
        }
        int exist = 0;
        char uname[50], pass[50];
        int login = 0;
        while(fscanf(file, "%s %s\n", uname, pass) > 0) {
            printf("Helllo %s %s", uname, pass);
            if (strcmp(username, uname)==0) {
                exist = 1;
                break;
            } 
        }

puts("eeee");

        char message[40];
        while(1) {
        if (exist == 0) {
            FILE *f = fopen("registeredUsers.txt", "a");
            if (f == NULL)
            {
                printf("Error opening file!\n");
                exit(1);
            }

            fprintf(f, "%s %s\n", username, password);
            fclose(f);

	puts("ooo");
                strcpy(message, "Successfully registered");
            break;
        } else {
            strcpy(message, "UserName already exists. Try other username.");
        }
    }
            if( send(client_sock , message , strlen(message) , 0) < 0)
            {
                puts("Send failed");
                return 1;
            }
        if(read_size == 0)
        {
            puts("Client disconnected");
            fflush(stdout);
        }
        else if(read_size == -1)
        {
         perror("recv failed");
        }
        exit(0);
    } else {
        close(client_sock);
    }
    }
     
    return 0;
}
