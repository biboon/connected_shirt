#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>

#include "http.h"

/* Main data structure */
static UdpData dataTab[NB_TEAMS];
static pthread_mutex_t dataMutex[NB_TEAMS];
static pthread_mutex_t webpageMutex;

static char teamsName[NB_TEAMS][25] = {"Jean & Flavien",
    "Cyril & JM",
    "Kevin & Benjamin",
    "Valentin & Alexander",
    "Timothee & Mageshwaran",
    "Jeremie & Julien",
    "Mehdi & Thibault",
    "Romain & Alexandre",
    "Sandra & Elise",
    "Hideo",
    "Arnaud"
};


void initMutex() {
    int i;
    for (i = 0; i < NB_TEAMS; i++) {
        pthread_mutex_init((dataMutex + i), NULL);
        pthread_mutex_unlock(dataMutex + i);
    }
    pthread_mutex_init(&webpageMutex, NULL);
    pthread_mutex_unlock(&webpageMutex);
}


void fillDataTab(int size, unsigned char* packet) {
    if (size == 5) {
        int team = (int) ((packet[0] & 0xF0) >> 4);
        pthread_mutex_lock(dataMutex + team);
        /* Putting data in structure */
        dataTab[team].i = team;
        dataTab[team].x = packet[1];
        dataTab[team].y = packet[2];
        dataTab[team].z = packet[3];
        dataTab[team].t = packet[4];
        /* Saving data in binary file */
        char filename[30];
        sprintf(filename, "./www/binaries/team_%d.bin", team);
        FILE* out = fopen(filename, "ab");
        fwrite((dataTab + team), sizeof(UdpData), 1, out);
        fclose(out);
        /* Finished */
        pthread_mutex_unlock(dataMutex + team);
    } else {
        fprintf(stderr, "Received packet with wrong size\n");
    }
    #ifdef DEBUG
        fprintf(stderr, "Finished processing UDP packet\n");
    #endif
}


void fillValeurs(FILE* client, FILE* webpage) {
    char buffer[MAX_BUFFER];
    int team = 0, cpt = 0;
    char request, byte;
    
    byte = fgetc(webpage);
    while ((byte != '<') && !feof(webpage) && (cpt < MAX_BUFFER)) {
        buffer[cpt] = byte;
        cpt++;
        byte = fgetc(webpage);
    }
    buffer[cpt] = '\0';
    
    if (sscanf(buffer, "team%d_%c", &team, &request) == 2) {
        pthread_mutex_lock(dataMutex + team);
        if (request == 'n')
            fprintf(client,"%s", teamsName[team]);
        else if (request == 'x')
            fprintf(client,"%d", dataTab[team].x);
        else if (request == 'y')
            fprintf(client, "%d", dataTab[team].y);
        else if (request == 'z')
            fprintf(client, "%d", dataTab[team].z);
        else if (request == 't')
            fprintf(client, "%d", dataTab[team].t);
        else if (request == 'i')
            fprintf(client, "%d", dataTab[team].i);
        pthread_mutex_unlock(dataMutex + team);
        fflush(client);
    } else
        fprintf(stderr, "fillValeurs : Bad request\n");
    fputc(byte, client); /* writes the < character */
}


void fillGraphes(FILE* client, FILE* webpage) {
    char buffer[MAX_BUFFER];
    int team = 0, cpt = 0, dataCnt = 0, status = 0;
    char byte;
    char filename[30];
    
    #ifdef DEBUG
        //fprintf(stderr, "fillGraphes function started\n");
    #endif
    
    byte = fgetc(webpage);
    while ((byte != ']') && !feof(webpage) && (cpt < MAX_BUFFER)) {
        buffer[cpt] = byte;
        cpt++;
        byte = fgetc(webpage);
    }
    buffer[cpt] = '\0';
    
    if (sscanf(buffer, "team_%d", &team) == 1) {
        /* Reading data in binary file */
        sprintf(filename, "./www/binaries/team_%d.bin", team);
        pthread_mutex_lock(dataMutex + team);
        FILE* in = fopen(filename, "rb");
        if (in != NULL) {
            UdpData *tmp = (UdpData*) malloc(sizeof(UdpData));
            if (tmp != NULL) {
                status = fread(tmp, sizeof(UdpData), 1, in);
                while (status == 1) {
                    if (dataCnt > 0) fprintf(client, ",\r\n");
                    //fprintf(client, "{ y: %d, a: %d, b: %d, c: %d, t: %d }", dataCnt, tmp->x, tmp->y, tmp->z, tmp->t);
                    fprintf(client, "vbntm");
                    fflush(client);
                    status = fread(tmp, sizeof(UdpData), 1, in);
                    dataCnt++;
                }
                free(tmp);
                //fflush(client);
            } else perror("fillGraphes.malloc failed");
            fclose(in);
        }
        pthread_mutex_unlock(dataMutex + team);
    } else
        fprintf(stderr, "fillGraphes : Bad request\n");
    
    fputc(byte, client); /* writes the ] character */
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
    
    if (fgets(buffer, MAX_BUFFER, client) == NULL) {
        fprintf(stderr, "Client connected on socket %d did not send any data\n", socket);
        return -1;
    }
    
    if (sscanf(buffer, "%s %s %s", cmd, page, proto) != 3) {
        fprintf(stderr, "Http request from client %d is not correctly formatted\n", socket);
        return -1;
    }
    
    while (fgets(buffer, MAX_BUFFER, client) != NULL)
        if (strcmp(buffer,"\r\n") == 0) break;
        
    if (strcmp(cmd, "GET") == 0) {
        if (strcmp(page, "/") == 0)
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
        
        pthread_mutex_lock(&webpageMutex);
        webpage = fopen(path, "r");
        if (webpage != NULL) {
            unsigned char byte;
            byte = fgetc(webpage);
            while (!feof(webpage)) {
                if (byte == '$') {
                    if (strcmp(page,"/valeurs.html") == 0) fillValeurs(client, webpage);
                    else if (strcmp(page,"/graphes.html") == 0) fillGraphes(client, webpage);
                } else fputc(byte, client);
                byte = fgetc(webpage);
            }
            fclose(webpage);
            fprintf(client, "\r\n");
            fflush(client);
            pthread_mutex_unlock(&webpageMutex);
        } else {
            perror("createHttpClient.fopen webpage");
            return -1;
        }
    } else fprintf(stderr, "Command not valid");
    
    #ifdef DEBUG
        printf("Http handling ended well, closing sock #%d\n", socket);
    #endif
    fclose(client);
    return 0;
}

