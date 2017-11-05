#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 1001

extern void customers(int );
extern void police(int );
extern void admin(int );

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, port, n;
    char buffer[MAX];
    
    struct sockaddr_in server_addr;
    struct hostent *server;

    if (argc < 3) {
       fprintf(stderr,"Usage: %s hostname port\n", argv[0]);
       exit(0);
    }

    port = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR in opening socket");
    
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR: Host not found\n");
        exit(0);
    }
    
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port);
    
    // connecting to server
    if (connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) 
        error("ERROR in connecting");
        
    char username[MAX];
    char password[MAX];
    char user_type;
    while(1)
    {
        // taking credentials from user   
        printf("Enter the credentials.\n");
        bzero(username,MAX);
        printf("Username: ");
        fgets(username, MAX, stdin);
            
        bzero(password,MAX);
        printf("Password: ");
        fgets (password, MAX, stdin);
        
        bzero(buffer,MAX);
        strcat(buffer,username);
        strcat(buffer,"$$$");
        strcat(buffer,password);

        // sending it to server
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
            error("ERROR writing to socket");
        
        // false or exit or success (gives user type)
        bzero(buffer,MAX);
        n = read(sockfd,buffer,MAX-1);
        if (n < 0) 
            error("ERROR reading from socket");
        
        if(!strcmp(buffer,"exit"))
        {
            printf("You entered the invalid credentials 3 times. Exiting...\n");
            return 0;
        }
        
        if(strcmp(buffer,"false"))
        {
            user_type = buffer[0];
            break;
        }
    }
    
    /* welcome to the bank */
    if(user_type=='C')
    {
        printf("Welcome Bank_Customer.\n");
        customers(sockfd);
    }
    else if(user_type=='A')
    {
        printf("Welcome Bank_Admin.\n");
        admin(sockfd);
    }
    else if(user_type=='P')
    {
        printf("Welcome Police.\n");
        police(sockfd);
    }

    // close the socket
    close(sockfd);  
    return 0;
}