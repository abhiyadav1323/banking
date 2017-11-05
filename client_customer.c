#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX 1001

extern void error(char *);

void customers(int sockfd)
{
    char buffer[MAX];
    char operation[MAX];
    int n;
    char flag;
    
    printf("Do u want to continue (y/n): ");
    scanf("%c",&flag);
    getchar();
    
    while(flag=='y')
    {
        bzero(buffer,MAX);
        buffer[0] = flag;
        buffer[1] = '\0';
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
            error("ERROR writing to socket");

        printf("Operation to perform: ");
        bzero(operation,MAX);
        fgets (operation, MAX, stdin);
        
        // sending command
        n = write(sockfd,operation,strlen(operation));
        if (n < 0) 
            error("ERROR writing to socket");
        
        operation[strlen(operation)-1] = '\0';

        // true or false
        bzero(buffer,MAX);
        n = read(sockfd,buffer,MAX-1);
        if (n < 0) 
            error("ERROR reading from socket");
        
        if(!strcmp(buffer,"false"))
        {
            printf("Invalid Operation.\n\n");
        }    
        else if(!strcmp(buffer,"true"))
        {
            if(!strcmp(operation,"balance"))
            {
                // delimiter string
                bzero(buffer,MAX);
                strcpy(buffer,"content");
                n = write(sockfd,buffer,strlen(buffer));
                if (n < 0) 
                    error("ERROR writing to socket");
                // balance
                bzero(buffer,MAX);
                n = read(sockfd, buffer, MAX-1);
                if (n < 0) 
                    error("ERROR reading from socket");
                printf("BALANCE: %s\n\n", buffer);   
            }
            else if(!strcmp(operation,"mini_statement"))
            {
                // delimeter string
                bzero(buffer,MAX);
                strcpy(buffer,"size");
                n = write(sockfd,buffer,strlen(buffer));
                if (n < 0) 
                    error("ERROR writing to socket");
                
                // file size 
                bzero(buffer,MAX);
                n = read(sockfd, buffer, MAX-1);
                if (n < 0) 
                    error("ERROR reading from socket");
                
                int file_size = atoi(buffer);
                int remain_data = file_size;
                
                // delimeter string
                bzero(buffer,MAX);
                strcpy(buffer,"content");
                n = write(sockfd,buffer,strlen(buffer));
                if (n < 0) 
                    error("ERROR writing to socket");

                // mini statement
                printf("MINI STATEMENT: \n");
                bzero(buffer,MAX);
                while ((remain_data > 0) && ((n = read(sockfd, buffer, MAX)) > 0))
                {
                    printf("%s", buffer);
                    remain_data -= n;
                    bzero(buffer,MAX);
                }
                printf("\n\n");
            }
        }
        printf("Do u want to continue (y/n): ");
        scanf("%c",&flag);
        getchar();
    }
    // sending flag
    bzero(buffer,MAX);
    buffer[0] = flag;
    buffer[1] = '\0';
    n = write(sockfd,buffer,strlen(buffer));  
}