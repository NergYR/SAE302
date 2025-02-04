# Définition des sous-répertoires
JAVA_DIR = RT3SAE02/ClientJava
SERVER_DIR = RT3SAE02/ClientServer
ANDROID_DIR = RT3SAE02/ClientAndroid

# Règle par défaut pour tout compiler
all: java server android

# Compilation du projet Java
java:
	@echo "=== Construction du client Java ==="
	@cd $(JAVA_DIR) && $(MAKE)

# Compilation du serveur
server:
	@echo "=== Construction du serveur et client en C ==="
	@cd $(SERVER_DIR) && $(MAKE) install

# Compilation du projet Android
android:
	@echo "=== Construction du client Android ==="
	@cd $(ANDROID_DIR) && $(MAKE)

# Nettoyage global
clean:
	@echo "=== Nettoyage global ==="
	@cd $(JAVA_DIR) && $(MAKE) clean
	@cd $(SERVER_DIR) && $(MAKE) clean
	@cd $(ANDROID_DIR) && $(MAKE) clean

# Aide
help:
	@echo "Commandes disponibles:"
	@echo "  make all     : Compile tous les projets"
	@echo "  make java    : Compile uniquement le client Java"
	@echo "  make server  : Compile uniquement le serveur"
	@echo "  make android : Compile uniquement le client Android"
	@echo "  make clean   : Nettoie tous les projets"
	@echo "  make help    : Affiche cette aide"

.PHONY: all java server android clean help
# Définition des sous-répertoires
JAVA_DIR = RT3SAE02/ClientJava
SERVER_DIR = RT3SAE02/ClientServer
ANDROID_DIR = RT3SAE02/ClientAndroid

# Règle par défaut pour tout compiler
all: java server android

# Compilation du projet Java
java:
	@echo "=== Construction du client Java ==="
	@cd $(JAVA_DIR) && $(MAKE)

# Compilation du serveur
server:
	@echo "=== Construction du serveur et client en C ==="
	@cd $(SERVER_DIR) && $(MAKE)

# Compilation du projet Android
android:
	@echo "=== Construction du client Android ==="
	@cd $(ANDROID_DIR) && $(MAKE)

# Nettoyage global
clean:
	@echo "=== Nettoyage global ==="
	@cd $(JAVA_DIR) && $(MAKE) clean
	@cd $(SERVER_DIR) && $(MAKE) clean
	@cd $(ANDROID_DIR) && $(MAKE) clean

# Aide
help:
	@echo "Commandes disponibles:"
	@echo "  make all     : Compile tous les projets"
	@echo "  make java    : Compile uniquement le client Java"
	@echo "  make server  : Compile uniquement le serveur"
	@echo "  make android : Compile uniquement le client Android"
	@echo "  make clean   : Nettoie tous les projets"
	@echo "  make help    : Affiche cette aide"

.PHONY: all java server android clean help
