


/*********************************************************************
 *                                                                   *
 * FICHIER: CLIENT_UDP                                               *
 *                                                                   *
 * DESCRIPTION: Utilisation de UDP socket par une application client *
 *                                                                   *
 * principaux appels (du point de vue client)                        *
 *     socket()                                                      *
 *                                                                   * 
 *     sendto()                                                      *
 *                                                                   *
 *     recvfrom()                                                    *
 *                                                                   *
 *********************************************************************/
#include     <stdio.h>
#include     <sys/types.h>
#include     <sys/socket.h>
#include     <netinet/in.h>
#include     <sys/un.h>
#include     <arpa/inet.h>
#include     <sys/uio.h>
#include     <time.h>
#include     <sys/timeb.h>
#include     <netdb.h>
#include     <stdlib.h>
#include     <strings.h>


#define GROUP "239.137.194.111"
#define PORT 55555
#define MAXLINE 80


void die (const char* message){
    perror(message);
    exit(1);
}


usage(){
    printf("usage : cliecho adresseIP_serveur(x.x.x.x)  numero_port_serveur\n");
}

int main (int argc, char *argv[]){

    int sds, port, ipaddress;
    char *data;
    struct timeb tp;
    struct sockaddr_in addr_s;
    
    /* Verifier le nombre de paramètre en entrée */
    /* clientUDP <hostname> <numero_port>        */
    if (argc != 3){
        usage();
        ipaddress = inet_addr(GROUP);
        port = PORT;
    } else {
        ipaddress = inet_addr(argv[1]);
        port = atoi(argv[2]);
    }

    /* 
    * Remplir la structure  addr_s avec l'adresse du groupe 
    */

    memset(&addr_s,0, sizeof(addr_s) );
    addr_s.sin_family = AF_INET;
    addr_s.sin_addr.s_addr = ipaddress;
    addr_s.sin_port = htons(port);

    /*
    * Ouvrir socket de groupe
    */
    if( ( sds = socket(AF_INET,SOCK_DGRAM,0) ) < 0) {
        die("socket> Unable to create socket");
    }


    unsigned char ttl= 1;

    if (setsockopt(sds, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) <0){
        die("setsockopt> Unable to set opt on sds socket");
    }


    ftime(&tp);
    data =ctime(&tp.time);

    
    int cnt,len_s = sizeof(addr_s);

    /* Emission de paquets */
    /* Utilisation de la primitive sendto */

    if ( (cnt = sendto(sds,data,strlen(data), 0, (struct sockaddr *)&addr_s, len_s ) ) < 0){
        die("Sendto> Unable to send buf");
    }
    close(sds);
}
