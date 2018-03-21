#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CMN_LEN 100

int main(int argc, char *argv[])
{
    /*char str[500];
    strcpy(str, system("ls"));
    printf("%s", str);*/
    system("openssl req -newkey rsa:2048 -days 3600 \
         -nodes -keyout server-key.pem -out server-req.pem\
         -subj '/C=IN/ST=NEW DELHI/L=Delhi/O=Example Brooklyn Company/CN=CA.com'");
    system("openssl rsa -in server-key.pem -out server-key.pem");
    return 0;
}