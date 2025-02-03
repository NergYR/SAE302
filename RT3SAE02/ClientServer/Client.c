#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "client_back.h"
#include "server.h"
#include "Client.h"

#define MAX_STUDENTS 100

// Fonction pour faire l'appel
void processAttendance(char* buffer, Student* students, int* nbStudents) {
    char *line;
    char *saveptr;
    char *tempBuffer = strdup(buffer);  // Create a copy of buffer
    *nbStudents = 0;

    if (!tempBuffer) {
        printf("[ERROR] Failed to allocate memory for tempBuffer\n");
        return;
    }

    printf("\nDébut de l'appel...\n");

    // Parcourir chaque ligne du fichier
    line = strtok_r(tempBuffer, "\n", &saveptr);
    while (line != NULL && *nbStudents < MAX_STUDENTS) {
        // Ignorer les lignes vides
        if (strlen(line) == 0) {
            line = strtok_r(NULL, "\n", &saveptr);
            continue;
        }

        // Ignorer la ligne d'en-tête si présente
        if (strstr(line, "etudid;") != NULL) {
            line = strtok_r(NULL, "\n", &saveptr);
            continue;
        }

        Student student;
        memset(&student, 0, sizeof(Student));

        // Nouvelle méthode de parsing avec strtok_r
        char *token;
        char *lineptr;
        char linecopy[256];
        strncpy(linecopy, line, sizeof(linecopy) - 1);
        linecopy[sizeof(linecopy) - 1] = '\0';

        int field = 0;
        token = strtok_r(linecopy, ";", &lineptr);
        while (token != NULL && field < 8) {
            switch(field) {
                case 0: strncpy(student.etudid, token, sizeof(student.etudid)-1); break;
                case 1: strncpy(student.nip, token, sizeof(student.nip)-1); break;
                case 2: strncpy(student.etat, token, sizeof(student.etat)-1); break;
                case 3: strncpy(student.civilite, token, sizeof(student.civilite)-1); break;
                case 4: strncpy(student.nom, token, sizeof(student.nom)-1); break;
                case 5: strncpy(student.nomusuel, token, sizeof(student.nomusuel)-1); break;
                case 6: strncpy(student.prenom, token, sizeof(student.prenom)-1); break;
                case 7: strncpy(student.tp, token, sizeof(student.tp)-1); break;
            }
            token = strtok_r(NULL, ";", &lineptr);
            field++;
        }

        if (field == 8) {
            printf("\n----------------------------------------");
            printf("\nÉtudiant %d:", *nbStudents + 1);
            printf("\nNom: %s %s", student.nom, student.prenom);
            printf("\nTP: %s", student.tp);
            printf("\nPrésent ? (o/n) : ");
            
            char reponse;
            scanf(" %c", &reponse);
            while (getchar() != '\n');

            student.presence = (reponse == 'o' || reponse == 'O') ? 'P' : 'A';
            students[*nbStudents] = student;
            (*nbStudents)++;
            printf("Présence enregistrée : %c\n", student.presence);
        } else {
            printf("[DEBUG] Skipping invalid line: %s (got %d fields)\n", line, field);
        }
        
        line = strtok_r(NULL, "\n", &saveptr);
    }

    free(tempBuffer);
    printf("\n----------------------------------------\n");
    printf("Appel terminé. Nombre d'étudiants traités : %d\n", *nbStudents);
}

// Déplacer la fonction createModifiedCSV ici
char* createModifiedCSV(Student* students, int nbStudents) {
    char* csv = malloc(BUFFER_SIZE * 10);
    if (csv == NULL) return NULL;
    
    // Ajouter l'en-tête
    strcpy(csv, "etudid;code_nip;etat;civilite_str;nom;nom_usuel;prenom;TP;presence\n");
    
    // Ajouter chaque étudiant
    for (int i = 0; i < nbStudents; i++) {
        char line[256];
        snprintf(line, sizeof(line), "%s;%s;%s;%s;%s;%s;%s;%s;%c\n",
                students[i].etudid, students[i].nip, students[i].etat,
                students[i].civilite, students[i].nom, students[i].nomusuel,
                students[i].prenom, students[i].tp, students[i].presence);
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