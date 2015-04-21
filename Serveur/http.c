#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "http.h"

/* Main data structure */
UdpData dataTab[11];


void fillDataTab(int size, unsigned char* packet) {
    if (size == 5) {
        int team = (int) ((packet[0] && 0xF0) >> 4);
        dataTab[team].x = packet[1];
        dataTab[team].y = packet[2];
        dataTab[team].z = packet[3];
        dataTab[team].t = packet[4];
    } else {
        fprintf(stderr, "Received packet with wrong size");
    }
    #ifdef DEBUG
        fprintf(stderr, "Finished processing UDP packet\n");
    #endif
}


void fillHtml(FILE* client, FILE* webpage) {
    char buffer[MAX_BUFFER];
    int team = 0, cpt = 0;
    char request, byte;
    
    byte = fgetc(webpage);
    while ((byte != '<') && !feof(webpage) && (cpt < MAX_BUFFER)) {
        buffer[cpt] = byte;
        cpt++;
        byte = fgetc(webpage);
    }
    
    if (sscanf(buffer, "team%d_%c", &team, &request) == 2) {
        if (request == 'x')
            fprintf(client,"%d", dataTab[team - 1].x);
        else if (request == 'y')
            fprintf(client, "%d", dataTab[team - 1].y);
        else if (request == 'z')
            fprintf(client, "%d", dataTab[team - 1].z);
        else if (request == 't')
            fprintf(client, "%d", dataTab[team - 1].t);
    } else
        fprintf(stderr, "fillHtml buffer error\n");
    fputc(byte, client); /* writes the < character */
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
    
    #ifdef DEBUG
        printf("Creating new http client on socket %d\n", socket);
    #endif
    
    client = fdopen(socket, "r+");
    if (client == NULL) {
        fprintf(stderr, "Failed to open the socket %d\n", socket);
        return -1;
    }
    
    if (fgets(buffer,MAX_BUFFER,client) == NULL) {
        fprintf(stderr, "Client connected on socket %d did not send any data\n", socket);
        return -1;
    }
    
    if (sscanf(buffer,"%s %s %s",cmd,page,proto) != 3) {
        fprintf(stderr, "Http request from client %d is not correctly formatted\n", socket);
        return -1;
    }
    
    while (fgets(buffer,MAX_BUFFER,client) != NULL)
        if (strcmp(buffer,"\r\n") == 0) break;
    
    if (strcmp(cmd, "GET")==0) {
        if (strcmp(page,"/") == 0)
            sprintf(page, "/index.html");
        
        int code = CODE_OK;
        struct stat fstat;
        sprintf(path, "%s%s", WEB_DIR, page);
        if (stat(path, &fstat) != 0 || !S_ISREG(fstat.st_mode)) {
            sprintf(path, "%s/%s", WEB_DIR, PAGE_NOTFOUND);
            code = CODE_NOTFOUND;
        }
        strcpy(type, "text/html");
        char *end = page + strlen(page);
        if (strcmp(end - 4, ".png") == 0) strcpy(type, "image/png");
        if (strcmp(end - 4, ".jpg") == 0) strcpy(type, "image/jpg");
        if (strcmp(end - 4, ".gif") == 0) strcpy(type, "image/gif");
        fprintf(client, "HTTP/1.0 %d\r\n", code);
        fprintf(client, "Server: CWeb\r\n");
        fprintf(client, "Content-type: %s\r\n", type);
        fprintf(client, "Content-length: %d\r\n", (int)fstat.st_size);
        fprintf(client, "\r\n");
        fflush(client);
        
        webpage = fopen(path, "r");
        if (webpage != NULL) {
            unsigned char byte;
            byte = fgetc(webpage);
            while (!feof(webpage)) {
                if (byte != '$') fputc(byte, client);
                else fillHtml(client, webpage);
                byte = fgetc(webpage);
            }
        } else {
            perror("createHttpClient.fopen webpage");
            return -1;
        }
    } else
        fprintf(stderr, "Command not valid");
    
    #ifdef DEBUG
        printf("Http handling ended well, closing sock #%d\n", socket);
    #endif
    fclose(client);
    return 0;
}

