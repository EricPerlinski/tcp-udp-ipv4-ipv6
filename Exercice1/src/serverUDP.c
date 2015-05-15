#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>



#define PORT 8000


/* Fonction die */
void die (const char* message) {
  perror(message);
  exit (EXIT_FAILURE);
}


/* Fonction de création de la socket */
int create_socket(u_short port)
{
  int sock,res_bind;
  struct sockaddr_in adresse;
  sock = socket(PF_INET,SOCK_DGRAM,0);
  if (sock < 0)
  {
    perror("Erreur d'ouverture de la socket");
    return(-1);
  }
  adresse.sin_family=PF_INET;
  adresse.sin_port=htons((u_short)port);
  adresse.sin_addr.s_addr=INADDR_ANY;

  res_bind=bind(sock,(struct sockaddr *)&adresse,sizeof(adresse));

  if(res_bind<0)
  {
    perror("Erreur d'association de la socket");
    return(-1);
  }
  return(sock);
}


int main(){

  u_short port=PORT;
  int sock = create_socket(port), received;
  struct sockaddr_in clientaddr;
  socklen_t socklen;
  char buffer[1024];

  printf("Démarrage du serveur...\n");
  printf("Serveur en attente de connexion...\n");
 
  for(;;){ /* pendant toute la durée de la vie d'un serveur */
  
    socklen = sizeof(clientaddr);
    memset(buffer,0,1024);
    if((received = recvfrom(sock,buffer,1024,0,(struct sockaddr *)&clientaddr,&socklen)) == -1) {
      die(strerror(errno));
    }
    
    if(sendto(sock,buffer,received,0,(struct sockaddr *)&clientaddr,sizeof(clientaddr)) == -1) {
      die(strerror(errno));
    }
    if(received>0){
      printf("Received the following: %s\n",buffer);
      printf("En attente de connexion...\n");
    }
  }

  return 0;
}