/** fichier libcom.h **/
/** Ce fichier decrit les structures et les constantes utilisees **/
/** par les fonctions de gestion des sockets                     **/

#ifndef __LIBCOM_H__
#define __LIBCOM_H__

/**** Constantes ****/
#define BUFSIZE 1024
#define MSG_LENGTH 5
#define UDP_PORT 12345

/**** Fonctions ****/
void afficheAdresse(FILE *flux, void *ip, int type); //Impression d'une adresse générale
void afficheAdresseSocket(FILE *flux, struct sockaddr_storage *padresse); //Impression d'une adresse de socket
void afficheHote(FILE *flux, struct hostent *hote, int type); //Impression des informations d'un hote
void messageUDP(char *hote, char *service, unsigned char *message, int taille); //Fonction d'envoi de message par UDP, hote: @serveur, service: port
int initialisationSocketUDP(char *service); //Fonction d'initialisation de serveur UDP, service: port
int boucleServeurUDP(int s, int (*traitement)(unsigned char *, int)); //Fonction de boucle serveur, message traite par fct traitement

#endif
