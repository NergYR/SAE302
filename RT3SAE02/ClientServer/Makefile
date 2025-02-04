# Détection du système d'exploitation
UNAME_S := $(shell uname -s)

# Définition des commandes et flags selon le système
ifeq ($(UNAME_S),Linux)
    # Détection de la distribution Linux
    DISTRO := $(shell ( lsb_release -si || cat /etc/redhat-release || echo Unknown ) 2>/dev/null | head -n1 | cut -d' ' -f1)
    RM = rm -f
    CC = gcc
    CFLAGS = -O2 -Wall
    LIB = -lm -lssh -lpthread
    INSTALL = install
    SUDO = sudo
    
    ifeq ($(DISTRO),Debian)
        INSTALL_DEPS = $(SUDO) apt-get update && $(SUDO) apt-get install -y build-essential libssh-dev
    else ifeq ($(DISTRO),Ubuntu)
        INSTALL_DEPS = $(SUDO) apt-get update && $(SUDO) apt-get install -y build-essential libssh-dev
    else ifeq ($(DISTRO),Kali)
        INSTALL_DEPS = $(SUDO) apt-get update && $(SUDO) apt-get install -y build-essential libssh-dev
    else ifneq (,$(filter $(DISTRO),RedHat CentOS Rocky))
        INSTALL_DEPS = $(SUDO) yum groupinstall -y "Development Tools" && $(SUDO) yum install -y libssh-devel
    endif
else ifeq ($(UNAME_S),Darwin)
    # Configuration MacOS
    RM = rm -f
    CC = clang
    CFLAGS = -O2 -Wall
    LIB = -lm -L/usr/local/opt/libssh/lib -lssh -lpthread
    INC = -I. -I/usr/local/opt/libssh/include
    INSTALL = ginstall
    SUDO = sudo
    INSTALL_DEPS = brew install libssh
endif

# Configuration commune
SRC_DIR_V2 = .

V2_SERVER_SRC = $(SRC_DIR_V2)/server.c $(SRC_DIR_V2)/unSecured_server.c \
                $(SRC_DIR_V2)/SSH_server.c $(SRC_DIR_V2)/ssh_tunnel.c $(SRC_DIR_V2)/engine.c

V2_CLIENT_COMMON_SRC = $(SRC_DIR_V2)/client_back.c $(SRC_DIR_V2)/Client.c
V2_SSH_CLIENT_SRC = $(SRC_DIR_V2)/SSH_client.c

V2_SERVER_OBJ = $(V2_SERVER_SRC:.c=.o)
V2_CLIENT_COMMON_OBJ = $(V2_CLIENT_COMMON_SRC:.c=.o)
V2_SSH_CLIENT_OBJ = $(V2_SSH_CLIENT_SRC:.c=.o)

V2_SERVER_EXEC = $(SRC_DIR_V2)/server
V2_CLIENT_EXEC = $(SRC_DIR_V2)/Client
V2_SSH_CLIENT_EXEC = $(SRC_DIR_V2)/SSH_client

# Flags de compilation conditionnelle
CLIENT_CFLAGS = $(CFLAGS) -DBUILD_CLIENT_MAIN
SSH_CLIENT_CFLAGS = $(CFLAGS)

all: check-deps $(V2_SERVER_EXEC) $(V2_CLIENT_EXEC) $(V2_SSH_CLIENT_EXEC)

check-deps:
	@echo "Vérification/Installation des dépendances..."
	@if [ -n "$(INSTALL_DEPS)" ]; then \
		$(INSTALL_DEPS); \
	else \
		echo "Distribution non reconnue. Veuillez installer manuellement les dépendances."; \
		exit 1; \
	fi
$(V2_SERVER_EXEC): $(V2_SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

$(V2_CLIENT_EXEC): $(V2_CLIENT_COMMON_SRC:.c=.client.o)
	$(CC) $(CLIENT_CFLAGS) -o $@ $^ $(LIB)

$(V2_SSH_CLIENT_EXEC): $(V2_SSH_CLIENT_SRC:.c=.o) $(V2_CLIENT_COMMON_SRC:.c=.ssh.o)
	$(CC) $(SSH_CLIENT_CFLAGS) -o $@ $^ $(LIB)

# Règles de compilation
$(SRC_DIR_V2)/%.o: $(SRC_DIR_V2)/%.c $(SRC_DIR_V2)/%.h
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

%.client.o: %.c
	$(CC) $(CLIENT_CFLAGS) $(INC) -c $< -o $@

%.ssh.o: %.c
	$(CC) $(SSH_CLIENT_CFLAGS) $(INC) -c $< -o $@

clean:
	$(RM) $(SRC_DIR_V2)/*.o 

install: all
	@echo "Installation des programmes..."
	$(SUDO) $(INSTALL) -m 755 $(V2_SERVER_EXEC) /usr/local/bin/server
	$(SUDO) $(INSTALL) -m 755 $(V2_CLIENT_EXEC) /usr/local/bin/Client
	$(SUDO) $(INSTALL) -m 755 $(V2_SSH_CLIENT_EXEC) /usr/local/bin/SSH_client
	@echo "Installation du fichier de configuration..."
	$(SUDO) mkdir -p /etc/ServerAppel/server
	$(SUDO) $(INSTALL) -m 644 config.conf /etc/ServerAppel/server/server.conf
	@echo "Installation terminée"
	@echo "Les fichiers du serveur sont dans /etc/ServerAppel/server/"

.PHONY: all clean check-deps install
