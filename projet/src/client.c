#include     <stdio.h>
#include     <unistd.h>
#include     <stdlib.h>
#include     <strings.h>

#include     <errno.h>
#include 	 <fcntl.h>
#include     <signal.h>

#include     <sys/types.h>
#include     <sys/socket.h>
#include     <sys/un.h>
#include     <sys/uio.h>
#include     <sys/timeb.h>

#include     <time.h>

#include     <netinet/in.h>
#include     <netdb.h>
#include     <arpa/inet.h>


#define SERV_PORT 5555
#define PORT 6567

#define MAXLINE 80
#define localhost "127.0.0.1"
#define GROUP "239.137.194.111"

int emetteurTab[FD_SETSIZE];
int jeton=0;

usage(){
    printf("usage : cliecho adresseIP_serveur(x.x.x.x)  numero_port_serveur\n");
}


void participateRequest(int *participate,int serverSocket,int *jeton) {
	int num;
	char buffer[10241]={};
	char mot[100]={};
	char s[100]={};
	printf(">>");
	scanf("%s",mot);
	if (strcmp(mot,"p")==0) {
		printf("wait a minute the server is checking if you can participate to the talk...\n");
		*participate=1;
		send(serverSocket, "p",1, 0);
		if (num = recv(serverSocket, buffer, 10241,0)!= -1) {
			if (strcmp("okn",buffer)==0){
				printf("the server accepted your participation to the lecture\n and there is %s in the buffer\n",buffer);
				//ajouter Ã  la confÃ©rence
			}else if (strcmp("okj",buffer)==0){
				*jeton=1;
				printf("the server accepted your participation to the lecture and you get the token\n,");
				//ajouter Ã  la confÃ©rence avec le jeton
			}
		}
	} else 	{
		printf("Sorry but you don't participate to the the lecture\n");
	}
}

void waitingMessage(int socketRecepteur, int serverSocket, struct sockaddr_in receveur_addr){
	fd_set rset,pset;
	int i=0;
	for(i=0;i<FD_SETSIZE;i++){
		emetteurTab[i]=-1;
	}
	FD_ZERO(&rset);
	FD_ZERO(&pset);
	FD_SET(serverSocket,&rset);
	FD_SET(socketRecepteur,&rset);
	int nbr=0;
	int goOn=1;
	int maxfd=FD_SETSIZE;
	while (goOn){
		pset=rset;
		nbr=select(maxfd, &pset, NULL , NULL , NULL);
		if(nbr>0){
			if(FD_ISSET(socketRecepteur,&pset)){
				int cnt,len_r=sizeof(receveur_addr);
				char buf[100]="\0";
				cnt=recvfrom(socketRecepteur,buf,sizeof(buf),0,(struct sockaddr *)&receveur_addr,&len_r);
				printf("%s\n",buf);
			}
			else if(FD_ISSET(serverSocket,&pset)){
				int num;
				char buffer[200]="\0";
				if (num = recv(serverSocket, buffer,sizeof(buffer),0)!= -1)
				{
					if (strcmp(buffer,"jeton")==0)
					{
						printf("%s\n",buffer);
						goOn=0;
						jeton=1;
					}
				}
			}
		}
	}
}

void sendMessage(int socketEmet,int serverSocket,struct sockaddr_in emetteur_addr){
	char mot[100]={};
	printf(">>");
	scanf("%s",mot);
	if ((strcmp(mot,"l") == 0)){
		send(serverSocket, "l",1, 0);
		sendto(socketEmet,"fin",3,0,(struct sockaddr *)&emetteur_addr, sizeof(emetteur_addr));
		jeton=0;
	}else if ((strcmp(mot,"v") == 0)){
		send(serverSocket, "v",1, 0);
	}else{
		int n= sendto(socketEmet, mot, strlen(mot)+1,0,(struct sockaddr *)&emetteur_addr, sizeof(emetteur_addr));
		if (n<0){
			perror ("erreur sendto");
			exit (1);
		}
	}

}

int main(int argc,char *argv[]) {

	struct sockaddr_in emetteur_addr,receveur_addr;
	int unixSocket, socketEmetteur,socketRecepteur;
	struct hostent *hp;  
	int ttl;
	struct ip_mreq imr;
	struct sockaddr_in  serv_addr;
	struct sockaddr_un  unixAddr;
	char *data;
	char sendbuf[1500];
	struct timeb tp;
	socklen_t len=sizeof(serv_addr);
		










    int serverSocket, servlen,n, retread,s;
    
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

















    /**socket pour multicast cote emetteur**/
	/* 
	* Remplir la structure  serv_addr avec l'adresse du serveur 
	*/
	memset( (char *) &emetteur_addr,0, sizeof(emetteur_addr) );
	emetteur_addr.sin_family = PF_INET;
	emetteur_addr.sin_addr.s_addr = inet_addr(GROUP);
	emetteur_addr.sin_port = htons(PORT);

	hp = (struct hostent *)gethostbyname (GROUP);
	if (hp == NULL) {

		exit(1);

	}
	//  bcopy( (char *) hp->h_addr,  (char *)& serv_addr.sin_addr, hp->h_length);
	memcpy( &emetteur_addr.sin_addr ,  hp->h_addr,  hp->h_length);
	//printf ("IP address: %s\n", inet_ntoa (serv_addr.sin_addr));


	/*
	* Ouvrir socket UDP
	*/
	if ((socketEmetteur = socket(PF_INET, SOCK_DGRAM, 0)) <0) {
		perror ("erreur socket");
		exit (1);
	}

	ttl=1;
	if (setsockopt(socketEmetteur, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) <0){
		perror ("setsockopt");
		exit (1);
	}
    /**-------------FIN initialisation emetteur----------**/

    /**socket pour multicast cote recepteur**/
	memset( (char *) &receveur_addr,0, sizeof(receveur_addr) );
	receveur_addr.sin_family = PF_INET;
	receveur_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	receveur_addr.sin_port = htons(PORT);

	/*creation socket*/
	if ((socketRecepteur = socket(PF_INET, SOCK_DGRAM, 0)) <0) {
		perror ("erreur socket");
		exit (1);
	}

	imr.imr_multiaddr.s_addr = inet_addr(GROUP);
	imr.imr_interface.s_addr =INADDR_ANY;

	if (setsockopt(socketRecepteur,IPPROTO_IP,IP_ADD_MEMBERSHIP, &imr, sizeof(imr)) <0){
		perror ("setsockopt1");
		exit (1);
	}

    // en cas de reutilisation d'un port
	unsigned int on=1;
	if (setsockopt(socketRecepteur,SOL_SOCKET,SO_REUSEADDR, &on, sizeof(on)) <0){
		perror ("setsockopt2");
		exit (1);
	}

	if (bind(socketRecepteur,(struct sockaddr *) &receveur_addr,sizeof(receveur_addr))<0){
		perror ("setsockopt3");
		exit (1);
	}
	/**-------------FIN initialisation recepteur----------**/


	/**instruction**/
	printf("now you are connected to the server\n");
	printf("====INSTRUCTION====\n");
	printf("press 'p' to participate to the lecturer\n");
	printf("press 'l' to let the token\n");
	printf("press 'd' to deconnect to the server\n");
	int participate=0;
	
	int count=0;
	for(;;)
	{


		if (!participate)
		{

			participateRequest(&participate,serverSocket,&jeton);
		}
		else 
		{
			if (!jeton)//si tu n'as pas de jeton tu ne parles pas tu attends un message
			{
				waitingMessage(socketRecepteur,serverSocket, receveur_addr);
			}
			else if (jeton)//si tu as un jeton tu parles
			{
				sendMessage(socketEmetteur,serverSocket,emetteur_addr);
			}
		}

	}
	close (serverSocket);


}