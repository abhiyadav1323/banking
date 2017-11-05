#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


#define MAX 1001

extern void error(char *);
extern char *client_ip;

int is_valid(char *amount)
{
	// checking validity of amount
	int i;
	int count=0;
	for(i=0;amount[i];i++)
	{
		if(amount[i]=='.')
		{
			count++;
			if(count>1)
				return 0;
		}		
		else if(amount[i]<='9'&&amount[i]>='0')
			continue;
		else
			return 0;
	}
	return 1;
}



void credit_amount(char *id, char *amount,char *trans)
{
	char filename[MAX];
	sprintf(filename,"%s.txt",id);

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

    double amt, amt_cred;
    sscanf(balance, "%lf", &amt);

    free(transaction);
    fclose(fp);

    // crediting amount
    sscanf(amount, "%lf", &amt_cred);
    amt += amt_cred;

    fp = fopen(filename,"a");
    if(fp == NULL)
        error("Error in opening user file for crediting.");
	
	time_t c_t = time(NULL);
	struct tm tm = *localtime(&c_t);
	fprintf(fp,"\n%.2d-%.2d-%.4d %s %f", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, trans, amt);
	fclose(fp);
}


int debit_amount(char *id, char *amount, char *trans)
{
	char filename[MAX];
	sprintf(filename,"%s.txt",id);

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
    
    double amt, req_amt;
    sscanf(balance, "%lf", &amt);

    free(transaction);
    fclose(fp);
 
    sscanf(amount, "%lf", &req_amt);
    if(amt<req_amt)
    	return 0;

    // debiting amount
    amt -= req_amt;

    fp = fopen(filename,"a");
    if(fp == NULL)
        error("Error in opening user file for debiting.");
	
	time_t c_t = time(NULL);
	struct tm tm = *localtime(&c_t);
	fprintf(fp,"\n%.2d-%.2d-%.4d %s %f", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, trans, amt);
	fclose(fp);            
	return 1;
}

void admin(int sockfd)
{
    int n;
    char buffer[MAX];
    char id[MAX], trans[MAX], amount[MAX];

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
        
        bzero(amount, MAX);
        bzero(id, MAX);
        bzero(trans, MAX);

        char *ptr = strtok(buffer,"$$$");
        strcpy(id,ptr);
        ptr = strtok(NULL, "$$$");
        strcpy(trans,ptr);
        ptr = strtok(NULL, "$$$");
        strcpy(amount,ptr);

        id[strlen(id)-1] = '\0';
        trans[strlen(trans)-1] = '\0';
        amount[strlen(amount)-1] = '\0';
        
        // checking for validity of user_id
        char *cred = NULL;
        size_t len = 0;
        int check = 0;
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
        if(check==0 || check==1 || (strcmp(trans,"credit") && strcmp(trans,"debit")) || !is_valid(amount))
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
        	if(!strcmp(trans,"credit"))
            {
            	credit_amount(id,amount,trans);
                fprintf(stdout, "Credit request from client with ip '%s' for customer '%s' successfully executed. \n", client_ip, id );
            	// sending true
                bzero(buffer,MAX);
                strcpy(buffer,"true");
                n = write(sockfd,buffer,strlen(buffer));  
                if (n < 0) 
                    error("ERROR writing to socket");
            }
            else if(!strcmp(trans,"debit"))
            {
            	int f = debit_amount(id, amount, trans);
                // sending true
                if(f==1)
                {
                	fprintf(stdout, "Debit request from client with ip '%s' for customer '%s' successfully executed. \n", client_ip, id );
                    bzero(buffer,MAX);
	                strcpy(buffer,"true");
	                n = write(sockfd,buffer,strlen(buffer));  
	                if (n < 0) 
	                    error("ERROR writing to socket");
                }
                else
                {
                	// insufficient amount
                	fprintf(stdout, "Debit request from client with ip '%s' declined. \n", client_ip);
                    bzero(buffer,MAX);
	                strcpy(buffer,"deficit");
	                n = write(sockfd,buffer,strlen(buffer));  
	                if (n < 0) 
	                    error("ERROR writing to socket");
                } 
            }
        }
        /* Reading flag */
        bzero(buffer,MAX);
        n = read(sockfd,buffer,MAX-1);
        if (n < 0) 
            error("ERROR reading from socket");
    }

}
