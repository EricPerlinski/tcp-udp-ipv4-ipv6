#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define SERV_PORT 2334
#define MAX_CLIENT 20
#define BUFSIZE 1500

int tab_clients[FD_SETSIZE];
int premier=0;
int leader=0;
int str_echo (int sockfd)
{
	int num, nsnd;
	char buffer[BUFSIZE];
	memset( (char*)buffer, 0, sizeof(buffer) );
	if ( (num= read ( sockfd, buffer, sizeof(buffer)-1) ) < 0 )  {
		perror ("servmulti : : readn error on socket");
		exit (1);
	}
	if (num !=-1) //on a reçu des données de la part du client
	{ 
		buffer[num] = '\0';
		if (strcmp(buffer,"p")==0)
		{
			printf("you press %s so the server is checking if you can participate to the lecture\n",buffer);
			printf("The client %d ask to participate to the lecture\n",sockfd);
			if (premier==0)//c'est le premier participant on lui donne le jeton par défaut
			{
				write(sockfd,"okj",3);
				leader=sockfd;
			}
			else //c'est un participant en plus on ne lui donne pas le jeton il devra le demander
			{
				write(sockfd,"okn",3);
			}

		}
		else if(strcmp(buffer,"l")==0)
		{
			printf("the client %d leave the token\n",sockfd);
			//passer le jeton en cherchant le prochain descripteur
			int placeSockfd =0;
			int j=0;
			for (j=0;j<FD_SETSIZE;j++)
			{
				if (tab_clients[j]==sockfd)
				{
					placeSockfd=j;
					break;
				}
			}
			int h=0;
			int nouveausockfd=0;
			int presentApres=0;
			for (h=placeSockfd+1;h<FD_SETSIZE;h++)
			{
				if(tab_clients[h]!=-1)
				{
					presentApres=1;
					nouveausockfd=tab_clients[h];
					break;
				}
			}
			if (presentApres==0)
			{
				h=0;
				for (h=0;h<FD_SETSIZE;h++)
				{
					if(tab_clients[h]!=-1)
					{
						nouveausockfd=tab_clients[h];
						break;
					}
				}
			}
			printf("j'envoie le jeton a %d\n",nouveausockfd);

			send (nouveausockfd,"jeton",5,0);
		}
	}
	else if (num == 0)
	{
		printf("Connection closed\n");
		return 0;
	}
	else 
	{
		perror("recv");
		exit(1);
	}
	return (nsnd);
}


usage(){
	printf("usage : servmulti numero_port_serveur\n");
}


int main (int argc,char *argv[])

{
	int sockfd, n, newsockfd, childpid, servlen,fin;
	struct sockaddr_in  serv_addr, cli_addr;
	socklen_t clilen;
	fd_set rset,pset;
	int maxfd;

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) <0) {
		perror("servmulti : Probleme socket\n");
		exit (2);
	}

	memset((char*) &serv_addr,0, sizeof(serv_addr));
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	serv_addr.sin_port = htons(SERV_PORT);


	if (bind(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr) ) <0) {
		perror ("servmulti : erreur bind\n");
		exit (1);
	}

	if (listen(sockfd, SOMAXCONN) <0) {
		perror ("servmulti : erreur listen\n");
		exit (1);
	}


	int i=0;
	for(i=0;i<FD_SETSIZE;i++)
	{
		tab_clients[i]=-1;
	}
	FD_ZERO(&rset);
	FD_ZERO(&pset);
	FD_SET(sockfd,&rset);
	int nbr=0;
	maxfd=sockfd+1;
	for (;;){
		pset=rset;
		nbr=select(maxfd, &pset, NULL , NULL , NULL);
		if(nbr>0)
		{
			
			if (FD_ISSET(sockfd,&pset)) 
			{
				clilen = sizeof(cli_addr);
				newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,  &clilen);
				if (newsockfd < 0) {
					perror("servmulti : erreur accept\n");
					exit (1);
				}
				i=0;
				while (i<FD_SETSIZE && tab_clients[i]>=0)
				{
					i++;
				}
				if (i== FD_SETSIZE)
				{
					exit(1);
				}
				tab_clients[i]=newsockfd;

				FD_SET(newsockfd,&rset);
				if (newsockfd >= maxfd)
				{
					maxfd=newsockfd+1;
				}
				nbr--;
			}

			i=0;
			while (i<FD_SETSIZE && nbr>0)
			{
				if (tab_clients[i]>=0 && FD_ISSET(tab_clients[i],&pset))
				{

					if (str_echo(tab_clients[i]) == 0)
					{
						close(tab_clients[i]);
						tab_clients[i]=-1;
						FD_CLR(tab_clients[i],&rset);
					}
					nbr--;
					premier=1;
				}
				i++;
			}
			;

		}

	}
	close(newsockfd);  

}



















