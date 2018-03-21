
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

#include <sys/socket.h>
#include <arpa/inet.h>

#include "chatroom_utils.h"

#define FILE_SIZE 1000000000



//SSL-Client.c
#include <malloc.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include<time.h>
 
#define FAIL    -1
 char clientusername[100];
 char port_number[50];

void waitFor (unsigned int secs) {
    unsigned int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}


int OpenConnection(const char *hostname, int port)
{   int sd;
    struct hostent *host;
    struct sockaddr_in addr;
  //puts("starting");
    if ( (host = gethostbyname(hostname)) == NULL )
    {
        perror(hostname);
        abort();
    }
    //puts("got hostname");
    //puts(port_number);
    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port_number));
    addr.sin_addr.s_addr = INADDR_ANY;
    //waitFor(3);
  //  puts("hey");
    if ( connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
    {
      puts("FAILED");
        close(sd);
        perror(hostname);
        abort();
    }
    return sd;
}
 
SSL_CTX* InitCTX(void)
{   SSL_METHOD *method;
    SSL_CTX *ctx;
 
    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */
    method = TLSv1_2_client_method();  /* Create new client-method instance */
    ctx = SSL_CTX_new(method);   /* Create new context */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}
 
void ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;
 
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if ( cert != NULL )
    {
        printf("Peer certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);       /* free the malloc'ed string */
        X509_free(cert);     /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No peer certificates configured.\n");
}


// TLS server

int OpenListener(int port)
{   int sd;
    struct sockaddr_in addr;
 
 //puts("ppp");
    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
   // puts("mml");
    //printf("%d", port);
    if ( bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
    {
        perror("can't bind port");
        abort();
    }
    if ( listen(sd, 10) != 0 )
    {
        perror("Can't configure listening port");
        abort();
    }
     //puts("ddd");
    return sd;
}
 
int isRoot()
{
    if (getuid() != 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
 
}
SSL_CTX* InitServerCTX(void)
{   SSL_METHOD *method;
    SSL_CTX *ctx;
 
    OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
    SSL_load_error_strings();   /* load all error messages */
    method = TLSv1_2_server_method();  /* create new server-method instance */
    ctx = SSL_CTX_new(method);   /* create new context from method */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}
 
void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
    /* set the local certificate from CertFile */
  //puts(CertFile);
  //puts(KeyFile);
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
   // puts("MMMMMLLLLLL");
}


 
 
void Servlet(SSL* ssl) /* Serve the connection -- threadable */
{   char buf[1024];
    char reply[1024];
    int sd, bytes;
    //const char* HTMLecho="<html><body><pre>%s</pre></body></html>\n\n";
 
    if ( SSL_accept(ssl) == FAIL )     /* do SSL-protocol accept */
        ERR_print_errors_fp(stderr);
    else
    {
        ShowCerts(ssl);        /* get any certificates */
        bytes = SSL_read(ssl, buf, sizeof(buf)); /* get request */
        if ( bytes > 0 )
        {
            buf[bytes] = 0;
            printf("Message: %s\n", buf);
           // sprintf(reply, HTMLecho, buf);   /* construct reply */
          //  SSL_write(ssl, reply, strlen(reply)); /* send reply */
        }
        else
            ERR_print_errors_fp(stderr);
    }
    sd = SSL_get_fd(ssl);       /* get socket connection */
    SSL_free(ssl);         /* release SSL state */
    close(sd);          /* close connection */
}

















// get a username from the user.
void get_username(char *username)
{
  while(true)
  {
    printf("Enter username and password separated by space <username password>: ");
    fflush(stdout);
    memset(username, 0, 1000);
    fgets(username, 22, stdin);
    //trim_newline(username);
    strcpy(clientusername, username);
    strcpy(clientusername, strtok(clientusername, " "));

    if(strlen(username) > 20)
    {
      puts("Username must be 20 characters or less.");
    } else {
      break;
    }
  }
}

void login_success(connection_info *connection, char *data) {
  //printf("LOGINN CHECK\n");
    if (strcmp(data,"0")==0) {
        printf("Wrong credentials \n"); 
        exit(1);     
    }
  
  printf("Login successfull\n");
}

//send local username to the server.
void set_username(connection_info *connection)
{
  message msg;
  msg.type = SET_USERNAME;
  strncpy(msg.username, connection->username, 20);

  if(send(connection->socket, (void*)&msg, sizeof(msg), 0) < 0)
  {
    perror("Send failed");
    exit(1);
  }
}

void stop_client(connection_info *connection)
{
  close(connection->socket);
  exit(0);
}

//initialize connection to the server.
void connect_to_server(connection_info *connection, char *address, char *port)
{

  while(true)
  {
    get_username(connection->username);

    //Create socket
    if ((connection->socket = socket(AF_INET, SOCK_STREAM , IPPROTO_TCP)) < 0)
    {
        perror("Could not create socket");
    }

    connection->address.sin_addr.s_addr = inet_addr(address);
    connection->address.sin_family = AF_INET;
    connection->address.sin_port = htons(atoi(port));

    //Connect to remote server
    if (connect(connection->socket, (struct sockaddr *)&connection->address , sizeof(connection->address)) < 0)
    {
        perror("Connect failed.");
        exit(1);
    }

    set_username(connection);
    break;
  }


  puts("Connected to server.");
  puts("Type /help for usage.");
}


void handle_user_input(connection_info *connection)
{
  char input[255];
  fgets(input, 255, stdin);
 // trim_newline(input);

  if(strcmp(input, "/q") == 0 || strcmp(input, "/quit") == 0)
  {
    stop_client(connection);
  }
  else if(strcmp(input, "/who") == 0)
  {
    message msg;
    msg.type = GET_USERS;

    if(send(connection->socket, &msg, sizeof(message), 0) < 0)
    {
        perror("Send failed");
        exit(1);
    }
  }
  else if(strcmp(input, "/help") == 0)
  {
    puts("/quit or /q: Exit the program.");
    puts("/help: Displays help information.");
    puts("/who: Displays list of users in chatroom.");
    puts("/msg <username> <message> Send a private message to given username.");
    puts("/create_grp <groupname> <members_separated_with_spaces> Create a new group with following members");
    puts("/join_grp <groupname> Join existing group.");
    puts("/send <username> <filename> Sends file to the user");
    puts("/msg_group <groupname> <messgae> Broadcasts message to the whole group");
    puts("/recv Join Receive all files you received and you didn't downloaded.");
  } else if (strncmp(input, "/msg_group", 10) == 0)
  { 
    message msg;
    msg.type = PUBLIC_MESSAGE;
    

    char *groupName;

    groupName = strtok(input+10, " ");

    if(strlen(groupName) > 20)
    {
      puts(KRED "The groupName must be between 1 and 20 characters." RESET);
      return;
    }

    if(groupName == NULL)
    {
      puts(KRED "You must enter a groupName." RESET);
      return;
    }

    // clear_stdin_buffer();

/*    if(strlen(input) == 0) {
        return;
    }
*/
    strncpy(msg.username, groupName, 20);
    strncpy(msg.data, strtok(NULL, ""), 255);

    //Send some data
    if(send(connection->socket, &msg, sizeof(message), 0) < 0)
    {
        perror("Send failed");
        exit(1);
    }
  } else if(strncmp(input, "/msg", 4) == 0)
  {
    message msg;
    msg.type = PRIVATE_MESSAGE;
    
    char *toUsername, *chatMsg;

    toUsername = strtok(input+5, " ");

    if(toUsername == NULL)
    {
      puts(KRED "The format for private messages is: /msg <username> <message>" RESET);
      return;
    }

    if(strlen(toUsername) == 0)
    {
      puts(KRED "You must enter a username for a private message." RESET);
      return;
    }

    if(strlen(toUsername) > 20)
    {
      puts(KRED "The username must be between 1 and 20 characters." RESET);
      return;
    }

    chatMsg = strtok(NULL, "");

    if(chatMsg == NULL)
    {
      puts(KRED "You must enter a message to send to the specified user." RESET);
      return;
    }

    if(strlen(chatMsg) > 100)
    {
      puts(KRED "You must enter a message less than 100 characters" RESET);
      return;
    }

    strncpy(msg.username, toUsername, 20);
    strncpy(msg.data, chatMsg, 255);

    if(send(connection->socket, &msg, sizeof(message), 0) < 0)
    {
        perror("Send failed");
        exit(1);
    }

    if(recv(connection->socket, &msg, sizeof(message), 0) < 0)
    {
        perror("Recv failed");
        exit(1);
    }
    puts("BHOOt");
    puts(msg.data);
 

    if (strlen(msg.data) < 10){
            strcpy(port_number, msg.data); 
        puts("START CONNECTION FOR SSL");
     SSL_CTX *ctx;
    int server;
    SSL *ssl;
    char buf[1024];
    int bytes;
    char hostname[50], portnum[50];
 
   /* if ( count != 3 )
    {
        printf("usage: %s <hostname> <portnum>\n", strings[0]);
        exit(0);
    }*/
    SSL_library_init();
    strcpy(hostname,"localhost");
    strcpy(portnum,port_number);
 
  puts("OPENING CONNECTION FOR SSL");
    ctx = InitCTX();
     SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);

    SSL_CTX_load_verify_locations(ctx, "CERTS/ca.pem", NULL);

    char cert[50];
    strcpy(cert, "CERTS/");
    strcat(cert, clientusername);
    strcat(cert, "-cert.pem");

    char key[50];
    strcpy(key, "CERTS/");
    strcat(key, clientusername);
    strcat(key, "-key.pem");
    //puts(cert);
    LoadCertificates(ctx, cert, key); /* load certs */

    server = OpenConnection(hostname, atoi(portnum));
   // puts("yeee");
    ssl = SSL_new(ctx);      /* create new SSL connection state */
    //LoadClientCertificates(ssl, cert, key); /* load certs */
    //ShowCerts(ssl);  
    //puts("BHAAAD");

    SSL_set_fd(ssl, server);    /* attach the socket descriptor */
    if ( SSL_connect(ssl) == FAIL )   /* perform the connection */
        ERR_print_errors_fp(stderr);
    else
    {   //char *msg = "Hello???";
 
        printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
        ShowCerts(ssl);        /* get any certs */
        SSL_write(ssl, chatMsg, strlen(chatMsg));   /* encrypt & send message */
       // bytes = SSL_read(ssl, buf, sizeof(buf)); /* get reply & decrypt */
        //buf[bytes] = 0;
        //printf("Received: \"%s\"\n", buf);
        
        SSL_free(ssl);        /* release connection state */
    }
    close(server);         /* close socket */
    SSL_CTX_free(ctx);        /* release context */
    }

    return 0;

  } else if (strncmp(input, "/send", 5) == 0) {              // create grp

    message msg;
    msg.type = SEND_FILE;
    char *username, *filename;

    username = strtok(input+6, " ");
    filename = strtok(NULL, "");


    if(username == NULL || filename == NULL || strlen(username) == 0 || strlen(filename) == 0)
    {
      puts(KRED "The format for send file is: /send <username> <filename>" RESET);
      return;
    }

    if (strlen(username) > 20) {
      puts(KRED "Username exceeds the length." RESET);
      return; 
    }

      if( access( filename, F_OK ) != -1 ) {
    
    char temp[30];
    strcpy(temp, filename);
    strtok(temp,".");
    char *fileType = strtok(NULL,"");

    //Get Picture Size
    printf("Getting file size.\n");
    FILE *picture;
    picture = fopen(filename, "r");
    long long size;
    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);

    //Send Picture Size
    printf("Sending file size. Size of file is :%lld\n", size);
    sprintf(msg.data, "%s %lld",fileType, size);
    strncpy(msg.username, username, 20);

    if (size > FILE_SIZE) {
      printf("File size out of limit. File size is limited to: %d and your file size is %lld\n", FILE_SIZE, size);
      return;
    }

    if(send(connection->socket, &msg, sizeof(message), 0) < 0)
    {
        perror("Send failed");
        exit(1);
    }
    //Send Picture as Byte Array
    printf("Sending Picture as Byte Array. Please wait.\n");
    char send_buffer[1026];
    while(!feof(picture)) {
    fread(send_buffer, 1, 1026, picture);
    write(connection->socket, send_buffer, 1026);
    bzero(send_buffer, 1026);
  }

      } else {
        printf("File don't exists\n");
      }


  } else if (strncmp(input, "/recv", 5) == 0) {              // join grp

    message msg;
    msg.type = RECEIVE_FILE;

    if(send(connection->socket, &msg, sizeof(message), 0) < 0)
    {
        perror("Send failed");
        exit(1);
    }

  } else if (strncmp(input, "/create_grp", 11) == 0) {              // create grp

    message msg;
    msg.type = CREATE_GROUP;
    char *groupName, *groupMembers;

    groupName = strtok(input+12, " ");
    groupMembers = strtok(NULL, "");

    if(groupMembers == NULL)
    {
      puts(KRED "The format for create group is: /create_grp <member1 member2 member 3 .... >" RESET);
      return;
    }

    if(strlen(groupMembers) == 0)
    {
      puts(KRED "You must add at least a member to the group." RESET);
      return;
    }

    if(strlen(groupName) > 20)
    {
      puts(KRED "The groupName must be between 1 and 20 characters." RESET);
      return;
    }

    if(groupName == NULL)
    {
      puts(KRED "You must enter a groupName." RESET);
      return;
    }

    strncpy(msg.username, groupName, 20);
    strncpy(msg.data, groupMembers, 255);

    if(send(connection->socket, &msg, sizeof(message), 0) < 0)
    {
        perror("Send failed");
        exit(1);
    }

  } else if (strncmp(input, "/join_grp", 9) == 0) {              // join grp

    message msg;
    msg.type = JOIN_GROUP;
    char *groupName;

    groupName = strtok(input+10, " ");

    if(strlen(groupName) > 20)
    {
      puts(KRED "The groupName must be between 1 and 20 characters." RESET);
      return;
    }

    if(groupName == NULL)
    {
      puts(KRED "You must enter a groupName." RESET);
      return;
    }

    strncpy(msg.username, groupName, 20);

    if(send(connection->socket, &msg, sizeof(message), 0) < 0)
    {
        perror("Send failed");
        exit(1);
    }

  } else {
    printf("This system doesn't understand this command. Type /help for help\n");
  }

}

void receive_file(connection_info *connection, char *username, char *message_text)
{



  char *fileType = strtok(message_text, " ");
 size_t size = atoi(strtok(NULL,""));
  printf("Reading Picture Byte Array %d\n", (int)size);
  int arraySize = 1026;
  char p_array[arraySize];
  size_t readSize = 0;
char str[30];


int i;
for (i = 0;; ++i) {
  strcpy(str, username);
  sprintf(str, "%s--%d", str, i);
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
int c= 0;
while (readSize <= size) {
  ++c;
    ssize_t result = read(connection->socket, p_array, arraySize);
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
}












void handlePrivateMsg(char por[50]) {

//puts("HANDLE PRIVATE MESSAGE");

  SSL_CTX *ctx;
    int server;
    char portnum[50];
 
    if(!isRoot())
    {
        printf("This program must be run as root/sudo user!!");
        exit(0);
    }
  /*  if ( count != 2 )
    {
        printf("Usage: %s <portnum>\n", strings[0]);
        exit(0);
    }*/
    SSL_library_init();
    //printf("%s", por);
    strcpy(portnum,por);
    ctx = InitServerCTX();        /* initialize SSL */
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);

    SSL_CTX_load_verify_locations(ctx, "CERTS/ca.pem", NULL);

    char cert[50];
    strcpy(cert, "CERTS/");
    strcat(cert, clientusername);
    strcat(cert, "-cert.pem");

    char key[50];
    strcpy(key, "CERTS/");
    strcat(key, clientusername);
    strcat(key, "-key.pem");
    LoadCertificates(ctx, cert, key); /* load certs */

    //puts("yoo");
    server = OpenListener(atoi(portnum));    /* create server socket */
 //   while (1)
    {   struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        SSL *ssl;
 
        int client = accept(server, (struct sockaddr*)&addr, &len);  /* accept connection as usual */
        printf("Connection: %s:%d\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        ssl = SSL_new(ctx);              /* get new SSL state with context */
        SSL_set_fd(ssl, client);      /* set connection socket to SSL state */
        Servlet(ssl);         /* service connection */
    }
    close(server);          /* close server socket */
    SSL_CTX_free(ctx);         /* release context */

}









void handle_server_message(connection_info *connection)
{
  message msg;
  //Receive a reply from the server
  ssize_t recv_val = recv(connection->socket, &msg, sizeof(message), 0);
  if(recv_val < 0)
  {   printf("MML\n");
      perror("recv failed");
      exit(1);

  }
  else if(recv_val == 0)
  {
    close(connection->socket);
    puts("Server disconnected.");
    exit(0);
  }

  switch(msg.type)
  {

    case CONNECT:
      printf(KYEL "%s has connected." RESET "\n", msg.username);
    break;

    case DISCONNECT:
      printf(KYEL "%s has disconnected." RESET "\n" , msg.username);
    break;

    case GET_USERS:
      printf(KYEL "%s" RESET "\n", msg.data);
    break;
    case PORT_MESSAGE:
        printf(KYEL "%s" RESET "\n", msg.data);
        strcpy(port_number, msg.data);
    break;

    case SET_USERNAME:
      //TODO: implement: name changes in the future?
    break;

    case PUBLIC_MESSAGE:
      printf(KGRN "%s" RESET ": %s\n", msg.username, msg.data);
    break;

    case PRIVATE_MESSAGE:
      printf(KWHT "From %s:" KCYN "\n" RESET, msg.username);
      //strcpy(port_number, msg.data);
      handlePrivateMsg(msg.data);
    break;

    case USERNAME_ERROR:
      printf(KWHT "%s:" KRED "\n" RESET, msg.data);
    break;

    case CREATE_GROUP:
      printf(KCYN " %s" RESET "\n",msg.data);
    break;

    case SUCCESS:
      login_success(connection, msg.data);
    break;

    case JOIN_GROUP:
      printf(KCYN " %s" RESET "\n",msg.data);
    break;

    case SEND_FILE:
      printf(KCYN " %s" RESET "\n",msg.data);
    break;

    case RECEIVE_FILE:
      receive_file(connection, msg.username, msg.data);
    break;

    case TOO_FULL:
      fprintf(stderr, KRED "Server chatroom is too full to accept new clients." RESET "\n");
      exit(0);
    break;

    default:
      fprintf(stderr, KRED "Unknown message type received." RESET "\n");
    break;
  }
}

int main(int argc, char *argv[])
{
  connection_info connection;
  fd_set file_descriptors;

  if (argc != 3) {
    fprintf(stderr,"Usage: %s <IP> <port>\n", argv[0]);
    exit(1);
  }

  
  if (strcmp(argv[2],"5555")==0) {

        int sock, sock2;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000];
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    //puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 5555 );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("You are now on our registeration page.\n");


  printf("Enter username and Password separated by space: ");
        
        char us[50];
        char pa[50];


        scanf("%s %s" , us, pa);
        strcpy(message, us);
        strcat(message, " ");
        strcat(message, pa);
        //puts(message);
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
puts("yuk");
    memset(server_reply, 0, sizeof server_reply);
    //puts("mm");
    if( recv(sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
        }
   // printf("%s", server_reply);  
  // puts("jaad"); 
puts(server_reply);
//puts("joo");
puts(strtok(message, " "));
char username[100];
strcpy(username, strtok(message, " "));
strcpy(clientusername, username);
  // creatting private key and CSR for CA
  char tempStr[200];
  strcpy(tempStr, "");
  strcat(tempStr, "openssl req -newkey rsa:2048 -days 3600 \
         -nodes -keyout CERTS/");
  strcat(tempStr, username);
  strcat(tempStr, "-key.pem -out CERTS/");
  strcat(tempStr, username);
  strcat(tempStr, "-req.pem -subj '/C=IN/ST=NEW DELHI/L=Delhi/O=Example Brooklyn Company/CN=client.com'");
  //puts(tempStr);
  system(tempStr);
  
  strcpy(tempStr, "openssl rsa -in CERTS/");
  strcat(tempStr, username);
  strcat(tempStr, "-key.pem -out CERTS/");
  strcat(tempStr, username);
  strcat(tempStr, "-key.pem");

//   system(server-key.pem -out server-req.pem\
///         -subj '/C=IN/ST=NEW DELHI/L=Delhi/O=Example Brooklyn Company/CN=client.com'");
  //  system("openssl rsa -in server-key.pem -out server-key.pem");
  //puts(tempStr);
  system(tempStr);
 close(sock);




 sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    //puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 6666 );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
 if( send(sock , username, strlen(username) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
puts("done");








    return 0;

  } else {
    connect_to_server(&connection, argv[1], argv[2]);
  //keep communicating with server
  while(true)
  { 
    FD_ZERO(&file_descriptors);
    FD_SET(STDIN_FILENO, &file_descriptors);
    FD_SET(connection.socket, &file_descriptors);
    fflush(stdin);

    if(select(connection.socket+1, &file_descriptors, NULL, NULL, NULL) < 0)
    {
      perror("Select failed.");
      exit(1);
    }

    if(FD_ISSET(STDIN_FILENO, &file_descriptors))
    {
      handle_user_input(&connection);
    }

    if(FD_ISSET(connection.socket, &file_descriptors))
    {
      handle_server_message(&connection);
    }
  }
}
  close(connection.socket);
  return 0;
}



