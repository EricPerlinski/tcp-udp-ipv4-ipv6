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

int str_echo (int sockfd, int nextsockcli) {
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

		}else if(strcmp(buffer,"ahc")==0){

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

		}else if(strcmp(buffer,"l")==0){
			printf("the client %d leave the token\n",sockfd);
			//passer le jeton en cherchant le prochain descripteur
			if(sockfd == nextsockcli){
				leader = sockfd;
				send (sockfd,"jeton",5,0);	
				printf("j'envoie le jeton a %d\n",sockfd);
			}else{
				leader = nextsockcli;
				send (nextsockcli,"jeton",5,0);
				printf("j'envoie le jeton a %d\n",nextsockcli);
			}
		}else if(strcmp(buffer,"d")==0){
			leader = nextsockcli;
			printf("the client %d wants to disconnect\n",sockfd);
			printf("Connection closed\n");
			send (nextsockcli,"jeton",5,0);
			send (sockfd,"okdeco",6,0);
			printf("j'envoie le jeton a %d\n",nextsockcli);
			return 0;
			
		}

	}else if (num == 0){
		leader = nextsockcli;
		printf("the client %d wants to disconnect\n",sockfd);
		printf("Connection closed\n");
		send (nextsockcli,"jeton",5,0);
		printf("j'envoie le jeton a %d\n",nextsockcli);
		return 0;
	}else{
		perror("recv");
		exit(1);
	}
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
	
	int k;

	for (;;){
		pset = rset;
        nbr = select(maxfd,&pset,NULL,NULL,NULL);

        if(FD_ISSET(serverSocket,&pset)){
            
            clilen = sizeof(cli_addr);
            clientSocket = accept(serverSocket, (struct sockaddr *) &cli_addr,  &clilen);
            
            i = 0;
            
            while((i<FD_SETSIZE) && (tab_clients[i] >= 0)) i++;
            k = i;
            
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
                int nextsockcli;
                int z;
                printf("k=%d;",k);
                for(z=0; z<10;z++) printf("%d:",tab_clients[z]);
                printf("end\n");

                if(k == 0){
                	nextsockcli = sockcli;
                }else if(i == k){
                	nextsockcli = tab_clients[0];
                }else{
                	nextsockcli = tab_clients[i+1];
                }

                if(str_echo(sockcli,nextsockcli) == 0){


                	int w = i;
                	k--;
                	while(tab_clients[w+1] > 0){
                		tab_clients[w] = tab_clients[w+1];
                		w++;
                	}
                	tab_clients[w]=-1;
                	printf("k=%d;",k);
                	for(z=0; z<10;z++) printf("%d:",tab_clients[z]);

                    close(sockcli);
                    FD_CLR(sockcli,&rset);

                }
                nbr--;
                
                premier=1;

                if(k==-1){
                		premier=0;
                }
                
            }
            i++;
        }
	} 
}

















