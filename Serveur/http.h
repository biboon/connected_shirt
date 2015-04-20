#ifndef __HTTP_H__
#define __HTTP_H__

typedef struct udpData {
	unsigned char x;
	unsigned char y;
	unsigned char z;
	unsigned char t;
} UdpData;

int createHttpClient(int socket);

void fillHtml(FILE* client, FILE* webpage);

#endif
