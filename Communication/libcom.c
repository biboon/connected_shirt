/** fichier libcom.c **/
/** Ce fichier contient des fonctions  **/
/**  concernant les sockets.           **/

/**** Fichiers d'inclusion ****/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "libcom.h"

#if 0
/**** Fonctions de gestion des sockets ****/
/** Impression d'une adresse generale **/
void afficheAdresse(FILE *flux, void *ip, int type) {
    char adresse[BUFSIZE];
    inet_ntop(type, ip, adresse, BUFSIZE);
    fprintf(flux, adresse);
}


/** Impression d'une adresse de socket **/
void afficheAdresseSocket(FILE *flux, struct sockaddr_storage *padresse) {
    void *ip;
    int port;
    if (padresse->ss_family == AF_INET) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *) padresse;
        ip = (void *) &ipv4->sin_addr;
        port = ipv4->sin_port;
    }
    if (padresse->ss_family == AF_INET6) {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) padresse;
        ip = (void *) &ipv6->sin6_addr;
        port = ipv6->sin6_port;
    }
    fprintf(flux, "Adresse IP%s : ", (padresse->ss_family == AF_INET) ? "v4" : "v6");
    afficheAdresse(flux, ip, padresse->ss_family);
    fprintf(flux, "\nPort de la socket : %d.\n", ntohs(port));
}


/** Impression des informations d'un hote **/
void afficheHote(FILE *flux, struct hostent *hote, int type) {
    char **params;

    fprintf(flux, "Nom officiel : '%s'.\n", hote->h_name);
    fprintf(flux, "Surnoms: ");
    for (params = hote->h_aliases; *params != NULL; params++) {
        fprintf(flux,"%s",*params);
        if (*(params + 1) == NULL) fprintf(flux, ",");
    }
    fprintf(flux, "\n");
    fprintf(flux, "Type des adresses   : %d.\n", hote->h_addrtype);
    fprintf(flux, "Taille des adresses : %d.\n", hote->h_length);
    fprintf(flux, "Adresses: ");
    for (params = hote->h_addr_list; params[0] != NULL; params++) {
        afficheAdresse(flux, (struct in_addr *)params, type);
        if((*params + 1) != NULL) fprintf(flux, ",");
    }
    fprintf(flux, "\n");
}
#endif

/**** Fonctions de serveur UDP ****/
/** fonction d'envoi de message par UDP, hote: @serveur, service: port **/
void messageUDP(char *hote, char *service, unsigned char *message, int taille) {
    struct addrinfo precisions, *resultat;
    int statut;
    int s;

    /* Creation de l'adresse de socket */
    memset(&precisions, 0, sizeof precisions);
    precisions.ai_family = AF_UNSPEC;
    precisions.ai_socktype = SOCK_DGRAM;
    statut = getaddrinfo(hote, service, &precisions, &resultat);
    if (statut < 0) { perror("messageUDPgenerique.getaddrinfo"); exit(EXIT_FAILURE); }

    /* Creation d'une socket */
    s = socket(resultat->ai_family, resultat->ai_socktype, resultat->ai_protocol);
    if (s < 0) { perror("messageUDPgenerique.socket"); exit(EXIT_FAILURE); }

    /* Option sur la socket */
    int vrai = 1;
    statut = setsockopt(s, SOL_SOCKET, SO_BROADCAST, &vrai, sizeof(vrai));
    if (statut < 0) { perror("initialisationServeurUDPgenerique.setsockopt (BROADCAST)"); exit(EXIT_FAILURE); }

    /* Envoi du message */
    int nboctets = sendto(s, message, taille, 0, resultat->ai_addr, resultat->ai_addrlen);
    if (nboctets < 0) { perror("messageUDPgenerique.sento"); exit(EXIT_FAILURE); }

    /* Liberation de la structure d'informations */
    freeaddrinfo(resultat);

    /* Fermeture de la socket d'envoi */
    close(s);
}


/** Fonction d'initialisation de serveur UDP, service: port **/
int initialisationSocketUDP(char *service) {
    struct addrinfo precisions, *resultat;
    int statut;
    int s;

    /* Construction de la structure adresse */
    memset(&precisions, 0, sizeof precisions);
    precisions.ai_family = AF_UNSPEC;
    precisions.ai_socktype = SOCK_DGRAM;
    precisions.ai_flags = AI_PASSIVE;
    statut = getaddrinfo(NULL, service, &precisions, &resultat);
    if (statut < 0) { perror("initialisationSocketUDP.getaddrinfo"); exit(EXIT_FAILURE); }

    /* Creation d'une socket */
    s = socket(resultat->ai_family, resultat->ai_socktype, resultat->ai_protocol);
    if (s < 0) { perror("initialisationSocketUDP.socket"); exit(EXIT_FAILURE); }

    /* Options utiles */
    int vrai = 1;
    statut = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &vrai, sizeof(vrai));
    if (statut < 0) { perror("initialisationServeurUDPgenerique.setsockopt (REUSEADDR)"); exit(EXIT_FAILURE); }

    /* Specification de l'adresse de la socket */
    statut = bind(s, resultat->ai_addr, resultat->ai_addrlen);
    if (statut < 0) {perror("initialisationServeurUDP.bind"); exit(EXIT_FAILURE); }

    /* Liberation de la structure d'informations */
    freeaddrinfo(resultat);

    return s;
}


/** Fonction de boucle serveur **/
int boucleServeurUDP(int s, int (*traitement)(unsigned char *, int)) {
    #ifdef DEBUG
        fprintf(stderr, "Started sever loop with fd: %d...\n", s);
    #endif
    while (1) {
        struct sockaddr_storage adresse;
        socklen_t taille = sizeof(adresse);
        unsigned char packet[MSG_LENGTH];
        #ifdef DEBUG
            fprintf(stderr, "Waiting for data to be received...\n");
        #endif
        int nboctets = recvfrom(s, packet, MSG_LENGTH, 0, (struct sockaddr *)&adresse, &taille);
        if (nboctets < 0) { perror("boucleServeurUDP.recvfrom"); exit(EXIT_FAILURE); }
        if (traitement(packet, nboctets) < 0) break;
    }
    return 0;
}


/** Fonction de demarrage de serveur UDP et d'ecoute **/
void serveurMessages(char *port, int (*traitement)(unsigned char *, int)) {
    #ifdef DEBUG
        fprintf(stderr, "Starting UDP messages server on port %s\n", port);
    #endif
    int sockUDP = initialisationSocketUDP(port);
    if (sockUDP < 0) { perror("serveurMessages.initialisationSocketUDP"); exit(EXIT_FAILURE); }
    #ifdef DEBUG
        fprintf(stderr, "Socket initialized on fd: %d\n", sockUDP);
    #endif
    boucleServeurUDP(sockUDP, traitement);
}


/**** Fonctions pour le serveur TCP ****/
/** Fonction d'initialisation de serveur TCP, connexions: max length of queue **/
int initialisationServeur(char *service, int connexions) {
    #ifdef DEBUG
        fprintf(stderr, "Creating TCP server socket on port %s...\n", service);
    #endif
    struct addrinfo precisions, *resultat;
    int status;
    int s;
    
    /* Building address structure */
    memset(&precisions, 0, sizeof precisions);
    precisions.ai_family = AF_UNSPEC;
    precisions.ai_socktype = SOCK_STREAM;
    precisions.ai_flags = AI_PASSIVE;
    status = getaddrinfo(NULL, service, &precisions, &resultat);
    if (status < 0) { perror("initialisationServeur.getaddrinfo"); exit(EXIT_FAILURE); }
    
    /* Creating socket */
    s = socket(resultat->ai_family, resultat->ai_socktype, resultat->ai_protocol);
    if (s < 0) { perror("initialisationServeur.socket"); exit(EXIT_FAILURE); }
    
    /* Useful options */
    int vrai = 1;
    status = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &vrai, sizeof(vrai));
    if (status < 0) { perror("initialisationServeur.setsockopt (REUSEADDR)"); exit(EXIT_FAILURE); }
    status = setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &vrai, sizeof(vrai));
    if (status < 0) { perror("initialisationServeur.setsockopt (NODELAY)"); exit(EXIT_FAILURE); }
    
    /* Socket address specification */
    status = bind(s, resultat->ai_addr, resultat->ai_addrlen);
    if (status < 0) exit(EXIT_FAILURE);
    
    /* Freeing informations structure */
    freeaddrinfo(resultat);
    
    /* Size of waiting queue */
    status = listen(s, connexions);
    if (status < 0) exit(EXIT_FAILURE);
    #ifdef DEBUG
        fprintf(stderr, "Socket succesfully created on fd: %d\n", s);
    #endif
    return s;
}


/* boucle du serveur TCP */
int boucleServeur(int ecoute, int (*traitement)(int)) {
    #ifdef DEBUG
        fprintf(stderr, "Started TCP server loop...\n");
    #endif
    while (1) {
        new_fd = accept(ecoute, NULL, NULL);
        if (new_fd < 0) { perror("boucleServeur.accept"); exit(EXIT_FAILURE); }
        else traitement(new_fd);
    }
    return 0;
}
