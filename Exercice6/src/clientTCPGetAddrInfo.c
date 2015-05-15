/*********************************************************************
 *                                                                   *
 * FICHIER: CLIENT_TCP                                               *
 *                                                                   *
 * DESCRIPTION: Utilisation de TCP socket par une application client *
 *                                                                   *
 * principaux appels (du point de vue client) pour un protocole      *
 * oriente connexion:                                                *
 *     socket()                                                      *
 *                                                                   * 
 *     connect()                                                     *
 *                                                                   *
 *     write()                                                       *
 *                                                                   *
 *     read()                                                        *
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

#include     "util.c"

#define MAXLINE 80

usage(){
    printf("usage : cliecho adresseIP_serveur(x.x.x.x)  numero_port_serveur\n");
}




int main (int argc, char *argv[]){

    int serverSocket, servlen, n, retread,s;
    
    char fromServer[MAXLINE];
    char fromUser[MAXLINE];

    struct addrinfo *result,*rp;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = 0;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    
    if (argc != 3){
        usage();
        exit(1);
    }

    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if( s != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(1);
    }

    /* Verifier le nombre de paramètre en entrée */
    /* clientTCP <hostname> <numero_port>        */
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        serverSocket = socket(rp->ai_family, rp->ai_socktype,
            rp->ai_protocol);
        if (serverSocket == -1)
            continue;

        if (connect(serverSocket, rp->ai_addr, rp->ai_addrlen) != -1)
            break; /* Success */

        close(serverSocket);
    }

    if (rp == NULL) { /* No address succeeded */
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    while ( (retread =readline (serverSocket, fromServer, MAXLINE)) > 0)
    {
        printf ("corr: %s", fromServer);
        if (strcmp (fromServer,"Au revoir\n") == 0) 
            break ; /* fin de la lecture */
      
        /* saisir message utilisateur */
        printf("vous: ");
        if (fgets(fromUser, MAXLINE,stdin) == NULL) {
            perror ("erreur fgets \n");
            exit(1);
        }
      
        /* Envoyer le message au serveur */
        if ( (n= writen (serverSocket, fromUser, strlen(fromUser)) ) != strlen(fromUser))  {
            printf ("erreur writen");
            exit (1);
        }
    }

    if(retread < 0 ) {
    	perror ("erreur readline \n");
    //	exit(1);
    }

    close(serverSocket);
}