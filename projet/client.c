#include     <stdio.h>
#include     <sys/types.h>
#include     <sys/socket.h>
#include     <netinet/in.h>
#include     <sys/un.h>
#include     <arpa/inet.h>
#include     <sys/uio.h>
#include     <time.h>
#include     <sys/timeb.h>
#include     <errno.h>
#include     <netdb.h>
#include     <stdlib.h>
#include     <strings.h>
#include 	 <fcntl.h>
#include     <signal.h>
#define SERV_PORT 2326
#define PORT 8000
#define localhost "127.0.0.1"
#define GROUP "239.137.194.111"

void participateRequest(int*,int,int*);
void waitingMessage(int);
void sendMessage(int);
void initialisationSocketEmetteur();
void initialisationSocketRecepteur();
void initialisationSocketServer();
struct sockaddr_in SockAddr;
struct sockaddr_in emetteur_addr;
struct sockaddr_in receveur_addr;
int serverSocket, unixSocket,  n, retread;
int socketEmetteur;
int socketRecepteur;

struct hostent *hp;  
int ttl;
struct ip_mreq imr;
struct sockaddr_in  serv_addr;
struct sockaddr_un  unixAddr;
char *data;
char sendbuf[1500];
struct timeb tp;
struct hostent *hp;  
int ttl;	
socklen_t len=sizeof(serv_addr);
int jeton=0;

int main(int argc,char *argv[])
{


    /**socket pour multicast cote emetteur**/
	initialisationSocketEmetteur();
    /**-------------FIN initialisation emetteur----------**/

    /**socket pour multicast cote recepteur**/
	initialisationSocketRecepteur();
	/**-------------FIN initialisation recepteur----------**/

	/**socket pour serveur**/
	initialisationSocketServer();
	/**-------------FIN initialisation serveur----------**/


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
				waitingMessage(socketRecepteur);
			}
			else if (jeton)//si tu as un jeton tu parles
			{
				sendMessage(socketEmetteur);
			}
		}

	}
	close (serverSocket);


}

void sig_urg(int sig)
{
  jeton=1;//tu es le leader maintenant
  printf("i got the token\n");
}

void participateRequest(int *participate,int serverSocket,int *jeton)
{
	int num;
	char buffer[10241]={};
	char mot[100]={};
	char s[100]={};
	printf(">>");
	scanf("%s",mot);
	if (strcmp(mot,"p")==0)
	{
		printf("wait a minute the server is checking if you can participate to the talk...\n");
		*participate=1;
		send(serverSocket, "p",1, 0);
		if (num = recv(serverSocket, buffer, 10241,0)!= -1)
		{
			if (strcmp("okn",buffer)==0)
			{
				printf("the server accepted your participation to the lecture\n and there is %s in the buffer\n",buffer);
				//ajouter à la conférence
			}
			else if (strcmp("okj",buffer)==0)
			{
				*jeton=1;
				printf("the server accepted your participation to the lecture and you get the token\n,");
				//ajouter à la conférence avec le jeton
			}
		}
	}
	else
	{
		printf("Sorry but you don't participate to the the lecture\n");
	}
}

void waitingMessage(int socketRecepteur)
{
	signal(SIGURG,sig_urg);
	fcntl(serverSocket, F_SETOWN, getpid());
	for(;;)
	{
		int cnt,len_r=sizeof(receveur_addr);
		char buf[100]="\0";
		cnt=recvfrom(socketRecepteur,buf,sizeof(buf),0,(struct sockaddr *)&receveur_addr,&len_r);
		if ((strcmp(buf,"fin") == 0))
		{
			return;
		}
		if(cnt<0)
		{
			perror("probleme recvfrom");
			exit(1);
		}
		else
		{
			printf("%s\n",buf);
		}
	}
}

void sendMessage(int socketEmet)
{
	char mot[100]={};
	printf(">>");
	scanf("%s",mot);
	if ((strcmp(mot,"l") == 0))
	{
		send(serverSocket, "l",1, 0);
		sendto(socketEmet,"fin",3,0,(struct sockaddr *)&emetteur_addr, sizeof(emetteur_addr));
		jeton=0;
	}
	else if ((strcmp(mot,"v") == 0))
	{
		send(serverSocket, "v",1, 0);
	}
	else
	{
		int n= sendto(socketEmet, mot, strlen(mot)+1,0,(struct sockaddr *)&emetteur_addr, sizeof(emetteur_addr));
		if (n<0)
		{
			perror ("erreur sendto");
			exit (1);
		}
	}

}

void initialisationSocketEmetteur()
{
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
 if (setsockopt(socketEmetteur, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) <0)
 {
 	perror ("setsockopt");
 	exit (1);
 }
}

void initialisationSocketRecepteur()
{
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

	if (setsockopt(socketRecepteur,IPPROTO_IP,IP_ADD_MEMBERSHIP, &imr, sizeof(imr)) <0)
	{
		perror ("setsockopt1");
		exit (1);
	}

    // en cas de réutilisation d'un port
	unsigned int on=1;
	if (setsockopt(socketRecepteur,SOL_SOCKET,SO_REUSEADDR, &on, sizeof(on)) <0)
	{
		perror ("setsockopt2");
		exit (1);
	}

	if (bind(socketRecepteur,(struct sockaddr *) &receveur_addr,sizeof(receveur_addr))<0)
	{
		perror ("setsockopt3");
		exit (1);
	}
}

void initialisationSocketServer()
{
		//socket pour discuter avec le serveur
	if ((serverSocket=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("erreur socket");
		exit(1);
	}

	bzero ((char *)&SockAddr, sizeof(SockAddr));
	SockAddr.sin_family =AF_INET;
	SockAddr.sin_port =htons(SERV_PORT);
	SockAddr.sin_addr.s_addr = inet_addr(localhost);
	/**connection au serveur**/
	if (connect(serverSocket,(struct sockaddr *)&SockAddr,sizeof(SockAddr))<0)
	{
		perror("error connection server");
		exit (1);
	}
}
