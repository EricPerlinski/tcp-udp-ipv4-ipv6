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
#include 	<arpa/nameser.h>
#include 	<resolv.h>

#define MAX_CLIENT 20
#define BUFSIZE 1500
#define MAXLINE 80


int premier=0;
int leader=0;

int str_echo (int sockfd) {
	int num, nsnd;
	char buffer[BUFSIZE];
	memset( (char*)buffer, 0, sizeof(buffer) );
	if ( (num= read ( sockfd, buffer, sizeof(buffer)-1) ) < 0 )  {
		perror ("servmulti : : readn error on socket");
		exit (1);
	}if (num !=-1){
	 //on a recu des donnees de la part du client
	 
		buffer[num] = '\0';
		if (strcmp(buffer,"p")==0){
			printf("you press %s so the server is checking if you can participate to the lecture\n",buffer);
			printf("The client %d ask to participate to the lecture\n",sockfd);
			if (premier==0){
			//c'est le premier participant on lui donne le jeton par dÃ©faut
				write(sockfd,"okj",3);
				leader=sockfd;
			}else{
			//c'est un participant en plus on ne lui donne pas le jeton il devra le demander
				write(sockfd,"okn",3);
			}

		}/*
		else if(strcmp(buffer,"l")==0){
			printf("the client %d leave the token\n",sockfd);
			//passer le jeton en cherchant le prochain descripteur
			int placeSockfd =0;
			int j=0;
			for (j=0;j<FD_SETSIZE;j++){
				if (tab_clients[j]==sockfd){
					placeSockfd=j;
					break;
				}
			}
			int h=0;
			int nouveausockfd=0;
			int presentApres=0;
			for (h=placeSockfd+1;h<FD_SETSIZE;h++){
				if(tab_clients[h]!=-1){
					presentApres=1;
					nouveausockfd=tab_clients[h];
					break;
				}
			}if (presentApres==0){
				h=0;
				for (h=0;h<FD_SETSIZE;h++){
					if(tab_clients[h]!=-1){
						nouveausockfd=tab_clients[h];
						break;
					}
				}
			}
			printf("j'envoie le jeton a %d\n",nouveausockfd);

			send (nouveausockfd,"jeton",5,0);
		}
	*/
	}
	/*
	else if (num == 0){
		printf("Connection closed\n");
		return 0;
	}else{
		perror("recv");
		exit(1);
	}
	*/
	return (nsnd);
}


usage(){
	printf("usage : servmulti numero_port_serveur\n");
}


int main (int argc,char *argv[]){

	int s,serverSocket, clientSocket,i,nbr; /* declaration socket passive et socket active */

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

	
	

	// en cas de reutilisation d'un port
	unsigned int on=1;
	if (setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR, &on, sizeof(on)) <0){
		perror ("setsockopt>ServerSocketReusable");
		exit (1);
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


    int childpid, servlen,fin,sockcli;
	socklen_t clilen;
	fd_set rset,pset;
	int maxfd;
	int tab_clients[FD_SETSIZE];
	maxfd=serverSocket+1;
	for(i=0;i<FD_SETSIZE;i++) tab_clients[i]=-1;
	FD_ZERO(&pset);
	FD_ZERO(&rset);
	FD_SET(serverSocket,&rset);
	
	

	for (;;){
		pset = rset;
        nbr = select(maxfd,&pset,NULL,NULL,NULL);

        if(FD_ISSET(serverSocket,&pset)){
            
            clilen = sizeof(cli_addr);
            clientSocket = accept(serverSocket, (struct sockaddr *) &cli_addr,  &clilen);
            
            i = 0;
            
            while((i<FD_SETSIZE) && (tab_clients[i] >= 0)) i++;
            
            if(i==FD_SETSIZE) exit(1);
            printf("--i= %d--\n",i+4);
            tab_clients[i] = clientSocket;
            FD_SET(clientSocket,&rset);

            if (clientSocket >= maxfd) maxfd= clientSocket+1;
            nbr--;
        }

        i=0;

        while((nbr > 0) && (i < FD_SETSIZE )){
            if(((sockcli = tab_clients[i]) >= 0) && (FD_ISSET(sockcli,&pset))) {
                printf("--sockcli= %d--\n",sockcli);
                if(str_echo(sockcli) == 0){
                    close(sockcli);
                    tab_clients[i] = -1;
                    FD_CLR(sockcli,&rset);
                    
                }
                nbr--;
                premier=1;
            }
            i++;
        }
	} 
}

















