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
    server.sin_port = htons( 6666 );
     
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
        puts(message_recieve);
        username = strtok(message_recieve, " ");
        password = strtok(NULL, " ");
        puts(message_recieve);
    

char command[200];
strcpy(command, "openssl x509 -req -in CERTS/");
strcat(command, username);
strcat(command, "-req.pem -days 3600 -CA CERTS/ca.pem -CAkey CERTS/ca-key.pem -set_serial 01 -out CERTS/");
strcat(command, username);
strcat(command, "-cert.pem");

system(command);


        
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
