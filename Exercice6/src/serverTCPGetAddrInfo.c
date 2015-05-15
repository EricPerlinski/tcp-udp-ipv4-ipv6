/*********************************************************************
 *                                                                   *
 * FICHIER: SERVER_TCP                                               *
 *                                                                   *
 * DESCRIPTION: Utilisation de TCP socket par une application serveur*
 *              application client                                   *
 *                                                                   *
 * principaux appels (du point de vue serveur) pour un protocole     *
 * oriente connexion:                                                *
 *     socket()                                                      *
 *                                                                   * 
 *     bind()                                                        *
 *                                                                   * 
 *     listen()                                                      *
 *                                                                   *
 *     accept()                                                      *
 *                                                                   *
 *     read()                                                        *
 *                                                                   *
 *     write()                                                       *
 *                                                                   *
 *********************************************************************/
#include     <stdio.h>
#include     <stdlib.h>
#include     <unistd.h>
#include     <strings.h>

#include     <errno.h>

#include     <sys/types.h>
#include     <sys/socket.h>
#include     <netinet/in.h>
#include     <sys/un.h>
#include     <arpa/inet.h>
#include     <sys/uio.h>
#include     <time.h>
#include     <sys/timeb.h>
#include     <netdb.h>

#include <arpa/nameser.h>
#include <resolv.h>

#include "util.c"

#define MAXLINE 80

usage(){
    printf("usage : servecho numero_port_serveur\n");
}


int main (int argc, char *argv[]){

    int s,serverSocket, clientSocket; /* declaration socket passive et socket active */

    struct addrinfo hints;
    struct addrinfo *result;
    struct sockaddr *cli_addr;
    struct sockaddr_storage peer_addr;
    
    memset(&hints,0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;


    int n, retread, childpid;
    socklen_t clilen;
   
    char fromClient[MAXLINE];
    char fromUser[MAXLINE];

    /* Verifier le nombre de paramètre en entrée */
    /* serverTCP <numero_port>                   */ 
    if (argc != 2){
        usage();
        exit(1);
    }

    s = getaddrinfo(NULL, argv[1], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }



    if ((serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) <0) {
        perror("servecho : Probleme socket\n");
        exit (2);
    }

    /*
    * Lier l'adresse  locale à la socket
    */
 
    if ( (bind(serverSocket,result->ai_addr, result->ai_addrlen) ) <0) {
        perror ("servecho: erreur bind\n");
        exit (1);
    }
  
    /* Paramètrer le nombre de connexion "pending" */
    if (listen(serverSocket, SOMAXCONN) <0) {
        perror ("servecho: erreur listen\n");
        exit (1);
    }

    freeaddrinfo(result);  




    clilen = sizeof(struct sockaddr_storage);
    clientSocket = accept(serverSocket, (struct sockaddr *) &cli_addr, &clilen);
    if (clientSocket < 0) {
        perror("servecho : erreur accept\n");
        exit (1);
    }


    /* Envoyer Bonjour au client */
    if ( (n= writen (clientSocket, "Bonjour\n", strlen("Bonjour\n")) ) != strlen("Bonjour\n"))  {
        printf ("erreur writen");
        exit (1);
    }

    while ( (retread=readline (clientSocket, fromClient, MAXLINE)) > 0){
        printf ("corr: %s", fromClient);
        if (strcmp (fromClient,"Au revoir\n") == 0) 
            break ; /* fin de la lecture */
      
        /* saisir message utilisateur */
        printf("vous: ");
        if (fgets(fromUser, MAXLINE,stdin) == NULL) {
            perror ("erreur fgets \n");
            exit(1);
        }
      
        /* Envoyer le message au client */
        if ( (n= writen (clientSocket, fromUser, strlen(fromUser)) ) != strlen(fromUser))  {
            printf ("erreur writen");
            exit (1);
        }

    }

    if(retread < 0 ) {
        perror ("erreur readline \n");
        //   exit(1);
    }

    close(serverSocket);
    close(serverSocket);


}
