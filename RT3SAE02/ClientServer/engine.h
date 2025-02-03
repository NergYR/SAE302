#ifndef ENGINE_H
#define ENGINE_H

#include "server.h"  // Pour BUFFER_SIZE
#include <stdio.h>

// Fonction pour le traitement du contenu du fichier
int process(char* buffer);

// Fonction pour lire et retourner le contenu d'un fichier
char* sendfile(const char* filename);

// Fonction pour sauvegarder le fichier modifié
int savefile(const char* filename, const char* content);

// Fonction pour vérifier si un fichier existe
int file_exists(const char* filename);

// Fonction pour obtenir l'extension d'un fichier
const char* get_file_extension(const char* filename);

#endif
