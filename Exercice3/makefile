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
SERVER = icmpd
CLIENT = client


#######################
##      REGLES       ##
#######################


#definition de la cible a reconstruire
all : $(DEST)$(SERVER)

#programme server
$(DEST)$(SERVER) : $(LISTE) $(SRC)$(SERVER).c
	@rm -f $(DEST)$(SERVER)
	$(CC) -o $(DEST)$(SERVER) $(SRC)$(SERVER).c

#programme client
$(DEST)$(CLIENT) : $(LISTE) $(SRC)$(CLIENT).c	
	@rm -f $(DEST)$(CLIENT)
	$(CC) -o $(DEST)$(CLIENT) $(SRC)$(CLIENT).c

#creation du dossier bin
directories:
	@mkdir -p $(DEST) 2>/dev/null

#destruction de tous les fichiers intermediaires crees
clean : 
	@rm -f $(DEST)*.o

#suppression de tout ce qui peut etre regenere
mrproper : clean
	@rm -f $(DEST)$(SERVER)
	@rm -f $(DEST)$(CLIENT)
