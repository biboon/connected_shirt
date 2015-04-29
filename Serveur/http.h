#ifndef __HTTP_H__
#define __HTTP_H__

/** Some constants **/
#define WEB_DIR  "./www"
#define PAGE_NOTFOUND "error.html"
#define MAX_BUFFER 1024

#define NB_TEAMS 11

#define CODE_OK  200
#define CODE_NOTFOUND 404

/** Definitions **/
typedef struct udpData {
	unsigned char x;
	unsigned char y;
	unsigned char z;
	unsigned char t;
	unsigned char i;
	long int ts;
} UdpData;

void fillValeurs(FILE* client, FILE* webpage);
void fillDataTab(int size, unsigned char* packet);
void fillGraphes(FILE* client, FILE* webpage);
int createHttpClient(int socket);

#endif
