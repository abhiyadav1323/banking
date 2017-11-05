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
extern void mini_statement(int, char *);
extern void available_balance(int, char *);
extern char *client_ip;

void police(int sockfd)
{
    int n;
    char buffer[MAX];
    char filename[100],op[MAX],id[MAX];

    // reading flag
    bzero(buffer,MAX);
    n = read(sockfd,buffer,MAX-1);
    if (n < 0) 
        error("ERROR reading from socket");
    
    while(buffer[0]=='y')
    {
    	int check = 0;
        // reading command
        bzero(buffer,MAX);
        n = read(sockfd,buffer,MAX-1);
        if (n < 0) 
            error("ERROR reading from socket");
        
        // breaking command into operation and user_id
        char *ptr = strtok(buffer,"$$$");
        strcpy(op,ptr);
        ptr = strtok(NULL,"$$$");
        strcpy(id,ptr);

        op[strlen(op)-1] = '\0';
        id[strlen(id)-1] = '\0';
        

        // checking for validity of user_id
        char *cred = NULL;
        size_t len = 0;
        
        FILE *fp = fopen("login_file.txt","r");
        if(fp == NULL)
            error("Error in opening login_file.");
        
        while(getline(&cred,&len,fp)!=-1)
        {
            char *username = strtok(cred," ");
            strtok(NULL," ");
            char *usertype = strtok(NULL, " ");
            
            if(!strcmp(username,id))
            {
                check=1;
                if(usertype[0]=='C')
                {
                    check=2;
                }
                break;
            } 
        }
        free(cred);
        fclose(fp);
        
        // sending false
        if(check==0 || check==1 || (strcmp(op,"balance") && strcmp(op,"mini_statement")))
        {
            fprintf(stdout, "Request from client with ip '%s' declined. \n", client_ip);    
            bzero(buffer,MAX);
            strcpy(buffer,"false");
            n = write(sockfd,buffer,strlen(buffer));  
            if (n < 0) 
                error("ERROR writing to socket");
        }
        else
        {
            if(!strcmp(op,"balance"))
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
            else if(!strcmp(op,"mini_statement"))
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
        }
        /* Reading flag */
        bzero(buffer,MAX);
        n = read(sockfd,buffer,MAX-1);
        if (n < 0) 
            error("ERROR reading from socket");
    }
    
}
