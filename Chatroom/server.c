/*
* PeerChat - a simple linux commandline Client - client chat
* Author: Tarun kumar yadav
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>

#include "chatroom_utils.h"

#define MAX_CLIENTS 4
int PORTS = 1111;
void initialize_server(connection_info *server_info, int port)
{
  if((server_info->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Failed to create socket");
    exit(1);
  }

  server_info->address.sin_family = AF_INET;
  server_info->address.sin_addr.s_addr = INADDR_ANY;
  server_info->address.sin_port = htons(port);

  if(bind(server_info->socket, (struct sockaddr *)&server_info->address, sizeof(server_info->address)) < 0)
  {
    perror("Binding failed");
    exit(1);
  }

  const int optVal = 1;
  const socklen_t optLen = sizeof(optVal);
  if(setsockopt(server_info->socket, SOL_SOCKET, SO_REUSEADDR, (void*) &optVal, optLen) < 0)
  {
    perror("Set socket option failed");
    exit(1);
  }


  if(listen(server_info->socket, 3) < 0) {
    perror("Listen failed");
    exit(1);
  }

  //Accept and incoming connection
  printf("Waiting for incoming connections...\n");
}

void send_public_message(connection_info clients[], int sender, char *group_name, char *message_text)
{
  message msg;
  msg.type = PUBLIC_MESSAGE;
  strncpy(msg.username, clients[sender].username, 20);
  strncpy(msg.data, message_text, 256);

  char line[512], *member;
  FILE *f= fopen("groups.txt", "r");
  while (fgets(line,sizeof(line), f)) {
    if(strcmp(strtok(line," "), group_name) == 0) {
      member = strtok(NULL, " ");

      while(member != NULL) {
        if (member[strlen(member) - 1] == '\n') {
          member[strlen(member) - 1] = '\0';
        }
          int i = 0;
          for(i = 0; i < MAX_CLIENTS; i++)
          { 
            if(i != sender && clients[i].socket != 0 && strcmp(clients[i].username, member)==0)
            { 
              if(send(clients[i].socket, &msg, sizeof(msg), 0) < 0)
              {
               perror("Send failed");
               exit(1);
              }
            }
          }
        member = strtok(NULL, " ");
      }
    }
  }
}

void send_private_message(connection_info clients[], int sender,
  char *username, char *message_text)
{
  message msg;
  msg.type = PRIVATE_MESSAGE;
  strcpy(msg.username, clients[sender].username);
sprintf(msg.data, "%d", PORTS);
//puts(msg.data);
PORTS = PORTS + 1; 

  int i;
  for(i = 0; i < MAX_CLIENTS; i++)
  {
    if(i != sender && clients[i].socket != 0
      && strcmp(clients[i].username, username) == 0)
    {
      if(send(clients[i].socket, &msg, sizeof(msg), 0) < 0)
      {
          perror("Send failed");
          exit(1);
      }

      if(send(clients[sender].socket, &msg, sizeof(msg), 0) < 0)
      {
          perror("Send failed");
          exit(1);
      }
      return;
    }
  }

  msg.type = USERNAME_ERROR;
  sprintf(msg.data, "Username \"%s\" does not exist or is not logged in.", username);

  if(send(clients[sender].socket, &msg, sizeof(msg), 0) < 0)
  {
      perror("Send failed");
      exit(1);
  }

}

void send_file(connection_info clients[], int sender,
  char *username, char *message_text)
{
  char *fileType = strtok(message_text, " ");
 size_t size = atoi(strtok(NULL,""));
  printf("Reading Picture Byte Array %d\n", (int)size);
  int arraySize = 1026;
  char p_array[arraySize];
  size_t readSize = 0;
  printf("KAKAKA\n");
char str[30];


int i;
for (i = 0;; ++i) {
  strcpy(str, username);
  sprintf(str, "%s%d", str, i);
  strcat(str, ".");
  strcat(str, fileType);
  if( access( str, F_OK ) != -1 ) {
    // file exists
} else {
  break;
}
}

FILE *image;
image = fopen(str, "w");
while (readSize < size) {
    ssize_t result = read(clients[sender].socket, p_array, arraySize);
    if (0 >= result)
    {
      if (0 == result)
      {
        fprintf(stderr, "The other end gracefully shut down the connection.\n");

        break;
      }
      else
      {
        if (EINTR == errno)  /* got interrupted, start over */
        {
           continue;
        }

        if (EAGAIN == errno)  /* in case reading from a non-blocking socket: no data available, start over */
        {
           continue;
        }

        /* Something went wrong unrecoverable. */
        perror("read() failed");

        break;
      }
    }
    else
    {
       readSize += result;
       if(0 == fwrite(p_array, 1, result, image)) {

        perror("Error while copying file\n");
       }
    }
}

fclose(image);

  message msg;
  msg.type = SEND_FILE;

  strcpy(msg.data, "File successfully sent.");

  if(send(clients[sender].socket, &msg, sizeof(msg), 0) < 0)
  {
      perror("Send failed");
      exit(1);
  }

}


void receive_file(connection_info clients[], int sender)      // client receives file
{

    message msg;
    msg.type = RECEIVE_FILE;
    char username[20];

    strcpy(username, clients[sender].username);




char str[30];
char temp[30];
int i = 0;
while(true) {

  strcpy(str, clients[sender].username);          // str is file Name on Server 
  sprintf(str, "%s%d", str, i);
  strcat(str, ".");
  //strcat(str, "png");

  strcpy(temp, str);
  bool found = false;

  strcat(temp, "png");
  if(found && access( temp, F_OK ) != -1 ) {
    strcpy(str, temp);
    found = true;
  }

  strcpy(temp, str);
  strcat(temp, "jpg");
  if(found && access( temp, F_OK ) != -1 ) {
    strcpy(str, temp);
  }

  strcpy(temp, str);
  strcat(temp, "jpeg");
  if(found && access( temp, F_OK ) != -1 ) {
    strcpy(str, temp);
  }

  strcpy(temp, str);
  strcat(temp, "pdf");
  if(found && access( temp, F_OK ) != -1 ) {
    strcpy(str, temp);
  }

  strcpy(temp, str);
  strcat(temp, "mkv");
  if(found && access( temp, F_OK ) != -1 ) {
    strcpy(str, temp);
  }



if( access( str, F_OK ) != -1 ) {
    // file exists
} else {
  break;
}
    char temp[30];
    strcpy(temp, str);
    strtok(temp,".");
    char *fileType = strtok(NULL,"");

    //Get Picture Size
    printf("Getting file Size %s\n", str);
    FILE *picture;
    picture = fopen(str, "r");
    int size;
    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);

    //Send Picture Size
    printf("Getting file Size\n");
    sprintf(msg.data, "%s %d",fileType, size);
    strncpy(msg.username, username, 20);

    if(send(clients[sender].socket, &msg, sizeof(message), 0) < 0)
    {
        perror("Send failed");
        exit(1);
    }
    //Send Picture as Byte Array
    printf("Sending file as Byte Array\n");
    char send_buffer[1026];
    int c=0;
    while(!feof(picture)) {
    fread(send_buffer, 1, 1026, picture);
    write(clients[sender].socket, send_buffer, 1026);
    bzero(send_buffer, 1026);
    ++c;
  }
++i;

  remove(str);
}

}


void create_group(connection_info clients[], int sender,
  char *username, char *message_text)
{
  message msg;
  msg.type = CREATE_GROUP;

  FILE *f = fopen("groups.txt", "a");
  if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

  fprintf(f, "%s %s %s\n", username, clients[sender].username, message_text);
  fclose(f);

  strncpy(msg.username, clients[sender].username, 20);
  strncpy(msg.data, message_text, 256);


  msg.type = CREATE_GROUP;
  sprintf(msg.data, "Group name \"%s\" successfully created with members: \"%s\"", username, message_text);

  if(send(clients[sender].socket, &msg, sizeof(msg), 0) < 0)
  {
      perror("Send failed");
      exit(1);
  }

}


void join_group(connection_info clients[], int sender, char *groupname)
{
  message msg;
  msg.type = JOIN_GROUP;

  FILE *original_file, *temporary_file;
  char *original_file_name = "groups.txt";
  char *temporary_file_name = "dummy.txt";
  original_file = fopen("groups.txt", "r");
  temporary_file = fopen(temporary_file_name, "w");
  char line[500], temp[500], *member;
  while (fgets(line,sizeof(line), original_file))
  {
      if (line[strlen(line) - 1] == '\n') {
        line[strlen(line) - 1] = '\0';
      }
      strcpy(temp, line);
      if(strcmp(strtok(temp," "), groupname) == 0) {

        member = strtok(NULL, " ");
        while(member != NULL) {
          if(strcmp(member, clients[sender].username) == 0) {     // if already in group, dont add
              fclose(temporary_file);
              fclose(original_file);
              remove(temporary_file_name);
              sprintf(msg.data, "You are already a member of: \"%s\"", groupname);
               if(send(clients[sender].socket, &msg, sizeof(msg), 0) < 0)
               {
                perror("Send failed");
                exit(1);
               }
               return;
          }
          member = strtok(NULL, " ");
        }

        strcat(line, " ");
        strcat(line, clients[sender].username);
        strcat(line, "\n");
      } else {
        strcat(line, "\n");
      }
      fputs(line, temporary_file);
  }

  fclose(temporary_file);
  fclose(original_file);
  rename(temporary_file_name, original_file_name);

  sprintf(msg.data, "You are now a member of: \"%s\"", groupname);

  if(send(clients[sender].socket, &msg, sizeof(msg), 0) < 0)
  {
      perror("Send failed");
      exit(1);
  }
}


void send_connect_message(connection_info *clients, int sender, int login)
{
//puts("hey");
  message msg;
  msg.type = SUCCESS;
  strncpy(msg.username, clients[sender].username, 21);
  int i = 0;
printf("LOGIN %d", login);
  for(i = 0; i <= MAX_CLIENTS; i++)
  {
    if(clients[i].socket != 0)
    {
      if(i == sender)
      {if (login == 1) {
        strcpy(msg.data, "1");
      } else{
        strcpy(msg.data, "0");
      }
        if(send(clients[i].socket, &msg, sizeof(msg), 0) < 0)
        {
            perror("Send failed");
            exit(1);
        }
      }
    }
  }
}

void send_disconnect_message(connection_info *clients, char *username)
{
  message msg;
  msg.type = DISCONNECT;
  strncpy(msg.username, username, 21);
  int i = 0;
  for(i = 0; i < MAX_CLIENTS; i++)
  {
    if(clients[i].socket != 0)
    {
      if(send(clients[i].socket, &msg, sizeof(msg), 0) < 0)
      {
          perror("Send failed");
          exit(1);
      }
    }
  }
}

void send_user_list(connection_info *clients, int receiver) {
  message msg;
  msg.type = GET_USERS;
  char *list = msg.data;

  int i;
  for(i = 0; i < MAX_CLIENTS; i++)
  {
    if(clients[i].socket != 0)
    {
      list = stpcpy(list, clients[i].username);
      list = stpcpy(list, "\n");
    }
  }

  if(send(clients[receiver].socket, &msg, sizeof(msg), 0) < 0)
  {
      perror("Send failed");
      exit(1);
  }

}

void send_too_full_message(int socket)
{
  message too_full_message;
  too_full_message.type = TOO_FULL;
  strcpy(too_full_message.data, "FULL");

  if(send(socket, &too_full_message, sizeof(too_full_message), 0) < 0)
  {
      perror("Send failed");
      exit(1);
  }
  close(socket);
}

//close all the sockets before exiting
void stop_server(connection_info connection[])
{
  int i;
  for(i = 0; i < MAX_CLIENTS; i++)
  {
    //send();
    close(connection[i].socket);
  }
  exit(0);
}

void login(connection_info *clients, int sender, char* loginDetails) {
        

        FILE *f = fopen("registeredUsers.txt", "r");
        if (f == NULL)
        {
            printf("Error opening file!\n");
            exit(1);
        }

        char *username = strtok(loginDetails, " ");
        char * password = strtok(NULL, "");
	password[strlen(password)-1] = '\0';
	printf("ss%sss", username);
	printf("kk%skk", password);
        char uname[50], pass[50];
        int login = 0;
        while(fscanf(f, "%s %s\n", uname, pass) > 0) {
            if (strcmp(username, uname)==0 && strcmp(password, pass) == 0) {
                login = 1;
                break;
            } 
        }

        fclose(f);

        strcpy(clients[sender].username, username);
        printf("\nUser connected: %s\n", clients[sender].username);
        send_connect_message(clients, sender, login);
//puts("mml");
        if (login != 1) {
            close(clients[sender].socket);
            clients[sender].socket = 0;
            
        }

}


void handle_client_message(connection_info clients[], int sender)
{
  int read_size;
  message msg;

  if((read_size = recv(clients[sender].socket, &msg, sizeof(message), 0)) == 0)
  {
    printf("User disconnected: %s.\n", clients[sender].username);
    close(clients[sender].socket);
    clients[sender].socket = 0;
    send_disconnect_message(clients, clients[sender].username);

  } else {

    switch(msg.type)
    {
      case GET_USERS:
        send_user_list(clients, sender);
      break;

      case SET_USERNAME:
        login(clients, sender, msg.username);
      break;

      case PUBLIC_MESSAGE:
        send_public_message(clients, sender, msg.username, msg.data);
      break;

      case PRIVATE_MESSAGE:
        send_private_message(clients, sender, msg.username, msg.data);
      break;

      case CREATE_GROUP:
        create_group(clients, sender, msg.username, msg.data);
      break;

      case JOIN_GROUP:
        join_group(clients, sender, msg.username);
      break;

      case SEND_FILE:
        send_file(clients, sender, msg.username, msg.data);
      break;

      case RECEIVE_FILE:
        receive_file(clients, sender);
      break;

      default:
        fprintf(stderr, "Unknown message type received.\n");
      break;
    }
  }
}

int construct_fd_set(fd_set *set, connection_info *server_info,
                      connection_info clients[])
{
  FD_ZERO(set);
  FD_SET(STDIN_FILENO, set);
  FD_SET(server_info->socket, set);

  int max_fd = server_info->socket;
  int i;
  for(i = 0; i < MAX_CLIENTS; i++)
  {
    if(clients[i].socket > 0)
    {
      FD_SET(clients[i].socket, set);
      if(clients[i].socket > max_fd)
      {
        max_fd = clients[i].socket;
      }
    }
  }
  return max_fd;
}

void handle_new_connection(connection_info *server_info, connection_info clients[])
{
  int new_socket;
  int address_len;
  new_socket = accept(server_info->socket, (struct sockaddr*)&server_info->address, (socklen_t*)&address_len);

  if (new_socket < 0)
  {
    perror("Accept Failed");
    exit(1);
  }

  int i;
  for(i = 0; i < MAX_CLIENTS; i++)
  {
    if(clients[i].socket == 0) {
      clients[i].socket = new_socket;
      break;

    } else if (i == MAX_CLIENTS -1) // if we can accept no more clients
    {
      send_too_full_message(new_socket);
    }
  }
}

void handle_user_input(connection_info clients[])
{
  char input[255];
  fgets(input, sizeof(input), stdin);
  trim_newline(input);

  if(input[0] == 'q') {
    stop_server(clients);
  }
}

int main(int argc, char *argv[])
{
  puts("Starting server.");

  fd_set file_descriptors;

  connection_info server_info;
  connection_info clients[MAX_CLIENTS];

  int i;
  for(i = 0; i < MAX_CLIENTS; i++)
  {
    clients[i].socket = 0;
  }

  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(1);
  }

  initialize_server(&server_info, atoi(argv[1]));

  while(true)
  {
    int max_fd = construct_fd_set(&file_descriptors, &server_info, clients);

    if(select(max_fd+1, &file_descriptors, NULL, NULL, NULL) < 0)
    {
      perror("Select Failed");
      stop_server(clients);
    }

    if(FD_ISSET(STDIN_FILENO, &file_descriptors))
    { 
      handle_user_input(clients);
    }

    if(FD_ISSET(server_info.socket, &file_descriptors))
    {
      handle_new_connection(&server_info, clients);
    }

    for(i = 0; i < MAX_CLIENTS; i++)
    {
      if(clients[i].socket > 0 && FD_ISSET(clients[i].socket, &file_descriptors))
      {
        handle_client_message(clients, i);
      }
    }
  }

  return 0;
}
