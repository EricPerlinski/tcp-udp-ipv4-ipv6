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

int main (int argc, char *argv[])

{
    int serverSocket, servlen, n, retread;
    struct sockaddr_in6  serv_addr;
    char server_addr_ipv6[100];
    char fromServer[MAXLINE];
    char fromUser[MAXLINE];
    struct hostent *hp;  


    /* Verifier le nombre de param�tre en entr�e */
    /* clientTCP <hostname> <numero_port>        */
    if (argc != 3){
        usage();
        exit(1);
    }


  
  /* 
   * Remplir la structure  serv_addr avec l'adresse du serveur 
   */
    memset ( (char *) &serv_addr, 0, sizeof(serv_addr) );
    serv_addr.sin6_flowinfo = 0;
    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_port = htons(atoi(argv[2]));

    hp = (struct hostent *)gethostbyname2 (argv[1],AF_INET6);
    if (hp == NULL) {
        fprintf(stderr, "%s: %s non trouve dans in /etc/hosts ou dans le DNS\n",
            argv[0], argv[1]);
        exit(1);
    }

    serv_addr.sin6_addr = * ((struct in6_addr *)(hp->h_addr));
    
    
    inet_ntop(AF_INET6, &(serv_addr.sin6_addr), server_addr_ipv6, 100);
    printf ("Server IP address: %s\n", server_addr_ipv6);
    /*
    * Ouvrir socket (socket STREAM)
    */
    if ((serverSocket = socket(AF_INET6, SOCK_STREAM, 0)) <0) {
        perror ("erreur socket");
        exit (1);
    }

    /*
    * Connect to the serveur 
    */
    if (connect (serverSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr) ) < 0){
        perror ("erreur connect");
        exit (1);
    }

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
