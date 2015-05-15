#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#define GROUP "239.137.194.111"
#define PORT 55555


/* Fonction die */
void die (const char* message) {
    perror(message);
    exit (EXIT_FAILURE);
}

usage(){
    printf("usage : servecho ipaddress numero_port\n");
}

int main(int argc, char* argv[]){

    int sdr;
    int ipaddress,port;
    struct sockaddr_in addr_r;

    if (argc != 3){
        usage();
        ipaddress = inet_addr(GROUP);
        port = PORT;
    } else {
        ipaddress = inet_addr(argv[1]);
        port = atoi(argv[2]);
    }


    /* Initialisation structure de reception */
    memset(&addr_r,0,sizeof(addr_r));
    addr_r.sin_family = AF_INET;
    addr_r.sin_port = htons(port);
    addr_r.sin_addr.s_addr = htonl(INADDR_ANY);

    /* creation socket */

    if( (sdr = socket(AF_INET, SOCK_DGRAM, 0) ) < 0){
        die("Socket> Unable to create socket");
    }

    /*********************************/
    /* Rejoindre le groupe multicast */
    /*********************************/

    struct ip_mreq imr;

    /* Initialisation de la structure imr */

    imr.imr_multiaddr.s_addr = ipaddress;
    imr.imr_interface.s_addr = INADDR_ANY;

    /* Demande de participation a un groupe */

    if( (setsockopt (sdr,IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr))) <0){
        die("Setsockopt> unable to require membership to group"); 
    }

    unsigned int on = 1;

    /* En cas de reutilisation d'un port */

    if( (setsockopt (sdr,SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) <0){
        die("Setsockopt> SOL_SOCKET, SO_REUSEADDR "); 
    }

    /* Nommer et lier le socket en reception */

    if(bind (sdr, (struct sockaddr *)&addr_r, sizeof(addr_r)) < 0 ){
        die("Bind> Unable to name and link the reception socket");
    }


    for(;;){
        int cnt, len_r = sizeof(addr_r);
        char recvbuf[2048];
        memset(recvbuf,0,2048);

        /* Reception de paquets */
        /* Utilisation de la primitive recvfrom */
        if ( (cnt = recvfrom(sdr,recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&addr_r, &len_r ) ) < 0){
            die("Sendto> Unable to send buf");
        }

        printf("Received : %s\n",recvbuf);
    }

    /* Demande pour abandonner le groupe */

    if( (setsockopt (sdr, IPPROTO_IP, IP_DROP_MEMBERSHIP, &imr, sizeof(imr))) <0){
        die("Setsockopt> IP_DROP_MEMBERSHIP"); 
    }


     
    return 0;
}