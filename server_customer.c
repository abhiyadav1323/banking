#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX 1001

extern void error(char *);
extern char *client_ip;

void available_balance(int sockfd,char *cust_id)
{
    int n;
    char filename[MAX];
    sprintf(filename,"%s",cust_id);
    strcat(filename,".txt");
    
    FILE *fp = fopen(filename,"r");
    if(fp == NULL)
        error("Error in opening user file for balance.");
    
    char *transaction = NULL;
    size_t len = 0;
    char *balance;

    while(getline(&transaction,&len,fp)!=-1)
    {
        strtok(transaction," ");
        strtok(NULL, " ");
        balance = strtok(NULL, " ");
    }
    
    fprintf(stdout, "Sending balance of customer '%s' to client with ip '%s'. \n", cust_id, client_ip);
    // balance
    n = write(sockfd,balance,strlen(balance));
    if (n < 0) 
        error("ERROR writing to socket");
    
    free(transaction);
    fclose(fp);
}

void mini_statement(int sockfd, char *cust_id)
{
    int n;
    char filename[MAX];
    sprintf(filename,"%s",cust_id);
    strcat(filename,".txt");
    struct stat file_stat;
    char buffer[MAX];

    
    int fd = open(filename, O_RDONLY);
    if(fd == -1)
        error("Error in opening user file for mini_statement");
    
    // finding stats of file
    if (fstat(fd, &file_stat) < 0)
        error("Error in getting statistics of file.");

    // writing size of file
    bzero(buffer,MAX);
    sprintf(buffer, "%d", (int)file_stat.st_size);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
        error("ERROR writing to socket");
    
    // delimeter string
    bzero(buffer,MAX);
    n = read(sockfd,buffer,MAX-1);
    if (n < 0) 
        error("ERROR reading from socket");
    
    // sending mini statement        
    fprintf(stdout, "Sending mini statement of customer '%s' to client with ip '%s'. \n", cust_id, client_ip);
    while (1) 
    {
        bzero(buffer,MAX);
        int bytes_read = read(fd, buffer, sizeof(buffer));
        if (bytes_read == 0) 
            break;
        if (bytes_read < 0) 
            error("ERROR reading from file.");
        
        void *ptr = buffer;
        while (bytes_read > 0) {
            int bytes_written = write(sockfd, ptr, bytes_read);
            if (bytes_written <= 0) 
                error("ERROR writing to socket");
            bytes_read -= bytes_written;
            ptr += bytes_written;
        }
    }
    close(fd);         
}


void customer(int sockfd,int cust_id)
{
    int n;
    char buffer[MAX];
    char id[MAX];
    sprintf(id,"%d",cust_id);

    /* Reading flag */
    bzero(buffer,MAX);
    n = read(sockfd,buffer,MAX-1);
    if (n < 0) 
        error("ERROR reading from socket");
    
    while(buffer[0]=='y')
    {   
        // reading command
        bzero(buffer,MAX);
        n = read(sockfd,buffer,MAX-1);
        if (n < 0) 
            error("ERROR reading from socket");
        
        buffer[strlen(buffer)-1] = '\0';
        
        if(!strcmp(buffer,"balance"))
        {
            // sending true
            bzero(buffer,MAX);
            strcpy(buffer,"true");
            n = write(sockfd,buffer,strlen(buffer));  
            if (n < 0) 
                error("ERROR writing to socket");
            
            // delimeter string
            bzero(buffer,MAX);
            n = read(sockfd,buffer,MAX-1);
            if (n < 0) 
                error("ERROR reading from socket");
            
            available_balance(sockfd,id);
        }
        else if(!strcmp(buffer,"mini_statement"))
        {
            // sending true
            bzero(buffer,MAX);
            strcpy(buffer,"true");
            n = write(sockfd,buffer,strlen(buffer));  
            if (n < 0) 
                error("ERROR writing to socket");
            
            // delimeter string
            bzero(buffer,MAX);
            n = read(sockfd,buffer,MAX-1);
            if (n < 0) 
                error("ERROR reading from socket");  
            
            mini_statement(sockfd,id); 
        }
        else
        {
            fprintf(stdout, "Request from client with ip '%s' declined. \n", client_ip);    
            // sending false
            bzero(buffer,MAX);
            strcpy(buffer,"false");
            n = write(sockfd,buffer,strlen(buffer));  
            if (n < 0) 
                error("ERROR writing to socket");
        } 
        /* Reading flag */
        bzero(buffer,MAX);
        n = read(sockfd,buffer,MAX-1);
        if (n < 0) 
            error("ERROR reading from socket");
    }
}