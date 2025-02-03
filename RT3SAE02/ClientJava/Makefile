# Variables
JAVAC = javac
JAVA = java
SOURCES = $(wildcard *.java)
CLASSES = $(SOURCES:.java=.class)
MAIN_CLASS = Main

# Règle par défaut
all: $(CLASSES)

# Compilation des fichiers .java en .class
%.class: %.java
	$(JAVAC) $<

# Exécution du programme
run: $(CLASSES)
	$(JAVA) $(MAIN_CLASS)

# Nettoyage des fichiers compilés
clean:
	rm -f *.class

# Aide
help:
	@echo "Commandes disponibles:"
	@echo "  make all    : Compile tous les fichiers Java"
	@echo "  make run    : Compile et exécute le programme"
	@echo "  make clean  : Supprime les fichiers compilés"
	@echo "  make help   : Affiche cette aide"

.PHONY: all run clean help
