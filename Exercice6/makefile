#######################
##     VARIABLES     ##
#######################


#definition du compilateur
CC = gcc

DIR = 
#$(shell pwd)/

SRC = $(DIR)src/

DEST = $(DIR)bin/

#specification des options du compilateur
CFLAGS = 

#librairies
LIB = 

#nom de l executable
SERVERV1 = serverTCPModifStruct
SERVERV2 = serverTCPGetAddrInfo
CLIENTV1 = clientTCPModifStruct
CLIENTV2 = clientTCPGetAddrInfo

#######################
##      REGLES       ##
#######################


#definition de la cible a reconstruire
all : $(DEST)$(SERVERV1) $(DEST)$(SERVERV2) $(DEST)$(CLIENTV1) $(DEST)$(CLIENTV2)

#programmes server
$(DEST)$(SERVERV1) : $(LISTE) $(SRC)$(SERVERV1).c
	@rm -f $(DEST)$(SERVERV1)
	$(CC) -o $(DEST)$(SERVERV1) $(SRC)$(SERVERV1).c

$(DEST)$(SERVERV2) : $(LISTE) $(SRC)$(SERVERV2).c
	@rm -f $(DEST)$(SERVERV2)
	$(CC) -o $(DEST)$(SERVERV2) $(SRC)$(SERVERV2).c

#programme client
$(DEST)$(CLIENTV1) : $(LISTE) $(SRC)$(CLIENTV1).c	
	@rm -f $(DEST)$(CLIENTV1)
	$(CC) -o $(DEST)$(CLIENTV1) $(SRC)$(CLIENTV1).c

$(DEST)$(CLIENTV2) : $(LISTE) $(SRC)$(CLIENTV2).c	
	@rm -f $(DEST)$(CLIENTV2)
	$(CC) -o $(DEST)$(CLIENTV2) $(SRC)$(CLIENTV2).c

#creation du dossier bin
directories:
	@mkdir -p $(DEST) 2>/dev/null

#destruction de tous les fichiers intermediaires crees
clean : 
	@rm -f $(DEST)*.o

#suppression de tout ce qui peut etre regenere
mrproper : clean
	@rm -f $(DEST)$(SERVERV1)
	@rm -f $(DEST)$(SERVERV2)
	@rm -f $(DEST)$(CLIENTV1)
	@rm -f $(DEST)$(CLIENTV2)

