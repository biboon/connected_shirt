
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "http.h"

/** Some constants **/

#define WEB_DIR  "./www"
#define PAGE_NOTFOUND "error.html"
#define MAX_BUFFER 1024

#define CODE_OK  200
#define CODE_NOTFOUND 404

UdpData dataTab[11];

void fillHtml(FILE* client, FILE* webpage)
{
	char buffer[MAX_BUFFER];
	int team = 0;
	char request;
	
	if(fgets(buffer,MAX_BUFFER,webpage)==NULL)
	{
		fprintf(stderr, "Could not read the webpage\n");
		return -1;
	}
	
	if (sscanf(buffer, "team%d_%c", &team, &request) == 2)
	{
		/*if (request == n)
			fprintf(client, UdpData[team-1].n);
		else */ if (request == x)
			fprintf(client, dataTab[team-1].x);
		else if (request == y)
			fprintf(client, dataTab[team-1].y);
		else if (request == z)
			fprintf(client, dataTab[team-1].z);
		else if (request == t)
			fprintf(client, dataTab[team-1].t);
	}
	else
		fprintf(stderr,"LELWTF\n");
}

/** Main procedure **/

int createHttpClient(int socket)
{
	char buffer[MAX_BUFFER];
	char cmd[MAX_BUFFER];
	char page[MAX_BUFFER];
	char proto[MAX_BUFFER];
	char path[MAX_BUFFER];
	char type[MAX_BUFFER];
	FILE* webpage = NULL;
	FILE* client = NULL;
	
	printf("Creating new http client on socket %d\n", socket);
	
	client = fdopen(socket, "r+");
	if (client == NULL)
	{
		fprintf(stderr, "Failed to open the socket %d\n", socket);
		return -1;
	}
	
	if(fgets(buffer,MAX_BUFFER,client)==NULL)
	{
		fprintf(stderr, "Client connected on socket %d did not send any data\n", socket);
		return -1;
	}
	
	if(sscanf(buffer,"%s %s %s",cmd,page,proto)!=3)
	{
		fprintf(stderr, "Http request from client %d is not correctly formatted\n", socket);
		return -1;
	}
	
	while(fgets(buffer,MAX_BUFFER,client)!=NULL)
		if(strcmp(buffer,"\r\n")==0)
			break;
	
	if (strcmp(cmd, "GET")==0)
	{
		if (strcmp(page,"/")==0)
			sprintf(page, "/index.html");
	    
	    int code=CODE_OK;
		struct stat fstat;
		sprintf(path,"%s%s",WEB_DIR,page);
		if(stat(path,&fstat)!=0 || !S_ISREG(fstat.st_mode))
		{
		    sprintf(path,"%s/%s",WEB_DIR,PAGE_NOTFOUND);
		    code=CODE_NOTFOUND;
	    }
		strcpy(type,"text/html");
		char *end=page+strlen(page);
		if(strcmp(end-4,".png")==0) strcpy(type,"image/png");
		if(strcmp(end-4,".jpg")==0) strcpy(type,"image/jpg");
		if(strcmp(end-4,".gif")==0) strcpy(type,"image/gif");
		fprintf(client,"HTTP/1.0 %d\r\n",code);
		fprintf(client,"Server: CWeb\r\n");
		fprintf(client,"Content-type: %s\r\n",type);
		fprintf(client,"Content-length: %d\r\n", (int)fstat.st_size);
		fprintf(client,"\r\n");
		fflush(client);
		
		webpage = fopen(path, "r");
		if (webpage != NULL)
		{
			unsigned char byte;
			byte = fgetc(webpage);
			while(!feof(webpage))
			{
				if (byte != '$')
				{
					fputc(byte, client);
					byte = fgetc(webpage);
				}
				else
					fillHtml(client, webpage);
			}
		}
	}
	else
		fprintf(stderr, "Command not valid");
	
	fclose(client);
	return 0;
}

