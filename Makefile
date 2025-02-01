RM = \rm -f
CC = gcc
CFLAGS = -O2 -Wall
INC = -I.
LIB = -lm -lssh -lpthread

INSTALLDIR = $(HOME)/bin

SRC_DIR_V2 = RT3SAE02/ClientServer

V2_SERVER_SRC = $(SRC_DIR_V2)/server.c $(SRC_DIR_V2)/unSecured_server.c $(SRC_DIR_V2)/SSH_server.c $(SRC_DIR_V2)/ssh_tunnel.c $(SRC_DIR_V2)/engine.c

# Séparer les objets pour éviter les conflits
V2_CLIENT_COMMON_SRC = $(SRC_DIR_V2)/client_back.c $(SRC_DIR_V2)/Client.c
V2_SSH_CLIENT_SRC = $(SRC_DIR_V2)/SSH_client.c

V2_SERVER_OBJ = $(V2_SERVER_SRC:.c=.o)
V2_CLIENT_COMMON_OBJ = $(V2_CLIENT_COMMON_SRC:.c=.o)
V2_SSH_CLIENT_OBJ = $(V2_SSH_CLIENT_SRC:.c=.o)

V2_SERVER_EXEC = $(SRC_DIR_V2)/server
V2_CLIENT_EXEC = $(SRC_DIR_V2)/Client
V2_SSH_CLIENT_EXEC = $(SRC_DIR_V2)/SSH_client

# Conserver les fichiers objets jusqu'à la fin de la compilation
.PRECIOUS: $(SRC_DIR_V2)/%.o

# Ajout des flags de compilation conditionnelle
CLIENT_CFLAGS = $(CFLAGS) -DBUILD_CLIENT_MAIN
SSH_CLIENT_CFLAGS = $(CFLAGS)

all: $(V2_SERVER_EXEC) $(V2_CLIENT_EXEC) $(V2_SSH_CLIENT_EXEC) clean

$(V2_SERVER_EXEC): $(V2_SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

$(V2_CLIENT_EXEC): $(V2_CLIENT_COMMON_SRC:.c=.client.o)
	$(CC) $(CLIENT_CFLAGS) -o $@ $^ $(LIB)

$(V2_SSH_CLIENT_EXEC): $(V2_SSH_CLIENT_SRC:.c=.o) $(V2_CLIENT_COMMON_SRC:.c=.ssh.o)
	$(CC) $(SSH_CLIENT_CFLAGS) -o $@ $^ $(LIB)

# Règle de compilation pour les fichiers objets
$(SRC_DIR_V2)/%.o: $(SRC_DIR_V2)/%.c $(SRC_DIR_V2)/%.h
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

%.client.o: %.c
	$(CC) $(CLIENT_CFLAGS) $(INC) -c $< -o $@

%.ssh.o: %.c
	$(CC) $(SSH_CLIENT_CFLAGS) $(INC) -c $< -o $@

# Nettoyage à la fin seulement
clean:
	$(RM) $(SRC_DIR_V2)/*.o 

install: all
	@echo "Installation des programmes..."
	sudo apt update && sudo apt install libssh-dev
	sudo install -m 755 $(V2_SERVER_EXEC) /usr/bin/server
	sudo install -m 755 $(V2_CLIENT_EXEC) /usr/bin/Client
	sudo install -m 755 $(V2_SSH_CLIENT_EXEC) /usr/bin/SSH_client
	@echo "Installation du fichier de configuration..."
	sudo mkdir -p /etc/ServerAppel/server
	sudo install -m 644 config.conf /etc/ServerAppel/server/server.conf
	@echo "Installation terminée"

.PHONY: all clean
