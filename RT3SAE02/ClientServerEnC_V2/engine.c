#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h>
#include "server.h"


char* sendfile(const char* filename) {
    char full_path[BUFFER_SIZE];
    snprintf(full_path, sizeof(full_path), "%s%s", FILE_PATH, filename);

    // Ajouter un message de débogage pour vérifier le chemin complet
    printf("[DEBUG] Chemin complet du fichier : %s\n", full_path);

    FILE *file = fopen(full_path, "r");
    if (file == NULL) {
        perror("Unable to open file");
        return NULL;
    }

    // Calculer la taille du fichier
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    printf("[DEBUG] Taille du fichier : %ld\n", file_size);

    // Allouer de la mémoire pour le contenu du fichier
    char *file_content = malloc(file_size + 1);
    if (file_content == NULL) {
        perror("Unable to allocate memory");
        fclose(file);
        return NULL;
    }

    // Lire le contenu du fichier
    size_t total_read = 0;
    size_t bytes_read;
    while ((bytes_read = fread(file_content + total_read, 1, file_size - total_read, file)) > 0) {
        total_read += bytes_read;
    }
    file_content[total_read] = '\0'; // Ajouter le caractère de fin de chaîne

    printf("[DEBUG] Total lu : %ld\n", total_read);

    fclose(file);
    return file_content;
}

int process(char* buffer) {
    printf("[DEBUG] buffer : %s\n", buffer);
    char* file_content = sendfile(buffer);
    if (file_content != NULL) {
        // Envoyer le contenu du fichier sur le socket ou le tunnel SSH
        printf("%s\n", file_content);
        free(file_content);
    } else {
        printf("Erreur lors de la lecture du fichier.\n");
    }
    return 0;
}