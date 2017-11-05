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

void admin(int sockfd)
{
	char buffer[MAX];
    char id[MAX],trans[MAX],amount[MAX];
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

        printf("User ID of Customer: ");
        bzero(id,MAX);
        fgets (id, MAX, stdin);
        
        printf("Transaction Type: ");
        bzero(trans,MAX);
        fgets (trans, MAX, stdin);

        printf("Amount: ");
        bzero(amount,MAX);
        fgets (amount, MAX, stdin);

        bzero(buffer,MAX);
        strcat(buffer,id);
        strcat(buffer,"$$$");
		strcat(buffer,trans);
        strcat(buffer,"$$$");
        strcat(buffer,amount);
        strcat(buffer,"$$$");

        // sending command
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
            error("ERROR writing to socket");
        
        // true or false
        bzero(buffer,MAX);
        n = read(sockfd,buffer,MAX-1);
        if (n < 0) 
            error("ERROR reading from socket");
        
        if(!strcmp(buffer,"false"))
        {
            printf("Transaction denied.\n\n");
        }    
        else if(!strcmp(buffer,"true"))
        {
            printf("Transaction successful.\n\n");
        }
        else if(!strcmp(buffer,"deficit"))
        {
        	printf("Insufficient Amount.\n\n");
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