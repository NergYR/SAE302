#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "client_back.h"

#define BUFFER_SIZE 4096
#define MAX_STUDENTS 100

// Structure pour stocker les informations des élèves
typedef struct {
    char name[256];
    char presence[20];
} Student;

// Fonction pour faire l'appel
void processAttendance(char* buffer, Student* students, int* nbStudents) {
    // Créer une copie du buffer pour la tokenisation
    char* buffer_copy = strdup(buffer);
    char* saveptr1 = NULL;  // Pour strtok_r
    char* line = strtok_r(buffer_copy, "\n", &saveptr1);
    *nbStudents = 0;
    char input[10];
    
    // Skip la première ligne (en-têtes)
    if (line != NULL) {
        line = strtok_r(NULL, "\n", &saveptr1);
    }
    
    while (line != NULL && *nbStudents < MAX_STUDENTS) {
        // Copie de la ligne pour la tokenisation
        char* temp_line = strdup(line);
        char* saveptr2 = NULL;  // Pour la deuxième tokenisation
        char* field = strtok_r(temp_line, ";", &saveptr2);
        char* nom = NULL;
        char* prenom = NULL;
        int field_count = 0;
        
        while(field != NULL && field_count <= 7) {  // Changé à 7 pour inclure le champ TP
            if(field_count == 4) nom = strdup(field);    
            if(field_count == 5) prenom = strdup(field); // Changé à 5 pour le prénom
            field = strtok_r(NULL, ";", &saveptr2);
            field_count++;
        }
        
        if(nom == NULL || prenom == NULL) {
            printf("Format de ligne incorrect\n");
            if(nom) free(nom);
            if(prenom) free(prenom);
            free(temp_line);
            line = strtok_r(NULL, "\n", &saveptr1);
            continue;
        }

        printf("\nPrésence de l'élève %s %s [absent] : ", nom, prenom);
        fflush(stdout);
        
        // Lecture de l'entrée utilisateur
        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = 0;
        } else {
            input[0] = '\0';
        }

        // Copie de la ligne complète
        strncpy(students[*nbStudents].name, line, 255);
        students[*nbStudents].name[255] = '\0';

        // Gestion présence/absence
        if (input[0] == ' ' || input[0] == '\0') {
            strcpy(students[*nbStudents].presence, "absent");
            printf("=> Absent\n");
        } else {
            strcpy(students[*nbStudents].presence, "present");
            printf("=> Présent\n");
        }

        free(nom);
        free(prenom);
        free(temp_line);
        (*nbStudents)++;
        line = strtok_r(NULL, "\n", &saveptr1);
    }
    
    free(buffer_copy);
}

// Fonction pour créer le CSV modifié
char* createModifiedCSV(Student* students, int nbStudents) {
    static char csv[BUFFER_SIZE * 10];
    memset(csv, 0, sizeof(csv));

    // En-tête du CSV
    strcat(csv, "etudid;code_nip;etat;civilite_str;nom;nom_usuel;prenom;TP;presence\n");

    for (int i = 0; i < nbStudents; i++) {
        char line[1024];  // Augmenter la taille du buffer pour éviter la troncature
        char* original = students[i].name;
        
        // Nettoyer la ligne
        original[strcspn(original, "\n")] = 0;
        original[strcspn(original, "\r")] = 0;

        // Traiter les caractères spéciaux
        char cleaned_line[512];
        int j, k = 0;
        for (j = 0; original[j] != '\0'; j++) {
            if ((unsigned char)original[j] >= 32 && (unsigned char)original[j] <= 126) {
                cleaned_line[k++] = original[j];
            }
        }
        cleaned_line[k] = '\0';
        
        // Assurer que la ligne ne dépassera pas la taille du buffer
        size_t max_len = sizeof(line) - strlen(students[i].presence) - 3; // -3 pour ";", "\n" et "\0"
        if (strlen(cleaned_line) > max_len) {
            cleaned_line[max_len] = '\0';
        }
        
        snprintf(line, sizeof(line), "%s;%s\n", cleaned_line, students[i].presence);
        strcat(csv, line);
    }

    return csv;
}

// Renommer la fonction main en client_main
int client_main(int argc, char *argv[]) {
    int portno;
    int sockfd;
    char buffer[BUFFER_SIZE];
    int n;

    // Vérification des arguments
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    // Conversion du port en entier
    portno = atoi(argv[2]);

    // Connexion au serveur
    if (connect_to_server(argv[1], portno, &sockfd) != 0) {
        exit(1);
    }
    
    // Lecture du message initial du serveur
    bzero(buffer, BUFFER_SIZE);
    n = read(sockfd, buffer, BUFFER_SIZE - 1);
    if (n < 0) {
        error("[ERROR] reading from socket");
    }
    printf("%s\n", buffer);

    // Demande du fichier à l'utilisateur
    printf("File Selected : ");
    bzero(buffer, BUFFER_SIZE);
    fgets(buffer, BUFFER_SIZE - 1, stdin);

    // Envoi du nom de fichier au serveur
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        error("[ERROR] writing to socket");
    }

    // Lecture de la réponse du serveur (contenu du fichier)
    bzero(buffer, BUFFER_SIZE);
    n = read(sockfd, buffer, BUFFER_SIZE - 1);
    if (n < 0) {
        error("[ERROR] reading from socket");
    }
    buffer[n] = '\0';

    Student students[MAX_STUDENTS];
    int nbStudents = 0;

    // Traitement de l'appel en local
    processAttendance(buffer, students, &nbStudents);

    printf("\nEnvoi du fichier modifié au serveur...\n");

    // Création et envoi du CSV modifié
    char* modifiedCSV = createModifiedCSV(students, nbStudents);
    printf("\n[DEBUG] Contenu du CSV à envoyer :\n%s\n", modifiedCSV);
    
    // Préparer le buffer final avec le marqueur
    char finalBuffer[BUFFER_SIZE * 10];
    memset(finalBuffer, 0, sizeof(finalBuffer));
    snprintf(finalBuffer, sizeof(finalBuffer), "%s\nEND_OF_FILE\n", modifiedCSV);
    
    // Envoyer en plusieurs parties si nécessaire
    size_t total_length = strlen(finalBuffer);
    size_t sent = 0;
    while (sent < total_length) {
        n = write(sockfd, finalBuffer + sent, 
                 (total_length - sent < BUFFER_SIZE) ? total_length - sent : BUFFER_SIZE);
        if (n < 0) {
            error("[ERROR] writing to socket");
            break;
        }
        sent += n;
    }

    // Attendre la confirmation du serveur
    bzero(buffer, BUFFER_SIZE);
    n = read(sockfd, buffer, BUFFER_SIZE - 1);
    if (n < 0) {
        error("[ERROR] reading from socket");
    }
    printf("%s", buffer);

    // Fermeture de la connexion
    close(sockfd);
    return 0;
}

// Ajouter une nouvelle fonction main qui appelle client_main
#ifdef BUILD_CLIENT_MAIN
int main(int argc, char *argv[]) {
    return client_main(argc, argv);
}
#endif