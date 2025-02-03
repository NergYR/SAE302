#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h>
#include "server.h" // pour fichesDirGlobal

#define BUFFER_SIZE 1024

char* sendfile(const char* filename) {
    printf("[DEBUG] Tentative d'ouverture du fichier: %s\n", filename);
    char* fileContent = malloc(BUFFER_SIZE * 10);
    if (!fileContent) {
        printf("[ERROR] Échec allocation mémoire\n");
        return NULL;
    }
    memset(fileContent, 0, BUFFER_SIZE * 10);
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("[ERROR] Impossible d'ouvrir le fichier");
        free(fileContent);
        return NULL;
    }

    // Lire et ignorer la première ligne (header)
    char buffer[BUFFER_SIZE];
    if (fgets(buffer, BUFFER_SIZE, file) == NULL) {
        printf("[ERROR] Erreur lecture header\n");
        fclose(file);
        free(fileContent);
        return NULL;
    }

    // Lire le reste du fichier
    size_t totalRead = 0;
    while (fgets(buffer, BUFFER_SIZE, file)) {
        size_t len = strlen(buffer);
        if (totalRead + len >= BUFFER_SIZE * 10) break;
        strcat(fileContent + totalRead, buffer);
        totalRead += len;
    }
    
    fclose(file);
    printf("[DEBUG] Fichier lu avec succès, taille: %zu octets\n", totalRead);
    return fileContent;
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