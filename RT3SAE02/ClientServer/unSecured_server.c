#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <time.h>

#define BUFFER_SIZE 1024

void doprocessing(int sock) {
    int n;
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    while(1) {  // Boucle principale pour maintenir la connexion
        DIR *d;
        struct dirent *dir;
        char file_list[BUFFER_SIZE] = "Liste des fichiers disponibles:\n";

        // Liste des fichiers
        d = opendir("fiches");
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                    strcat(file_list, dir->d_name);
                    strcat(file_list, "\n");
                }
            }
            closedir(d);
        } else {
            perror("ERROR opening directory");
            return;
        }

        // Envoi de la liste et du message de prompt
        strcat(file_list, "\nEntrez le nom du fichier (ou 'quit' pour quitter) : ");
        n = write(sock, file_list, strlen(file_list));
        if (n < 0) {
            perror("ERROR writing to socket");
            return;
        }

        // Lecture du fichier demandé
        bzero(buffer, BUFFER_SIZE);
        n = read(sock, buffer, BUFFER_SIZE-1);
        if (n < 0) {
            perror("ERROR reading from socket");
            return;
        }

        buffer[strcspn(buffer, "\r\n")] = 0;

        // Vérifier si le client veut quitter
        if (strcmp(buffer, "quit") == 0) {
            write(sock, "Au revoir!\n", 11);
            break;
        }

        // Ouverture et envoi du fichier demandé
        char filepath[BUFFER_SIZE];
        snprintf(filepath, sizeof(filepath), "fiches/%s", buffer);
        
        FILE *file = fopen(filepath, "r");
        if (file == NULL) {
            char *error_msg = "Fichier non trouvé.\n\n";
            write(sock, error_msg, strlen(error_msg));
            continue;
        }

        // Envoi du contenu du fichier
        while ((n = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
            write(sock, buffer, n);
        }
        fclose(file);

        // Attendre la réception du fichier modifié
        char complete_buffer[BUFFER_SIZE * 10] = {0};
        int total_received = 0;
        memset(complete_buffer, 0, sizeof(complete_buffer));
        
        // Utiliser un buffer temporaire pour chaque lecture
        char temp_buffer[BUFFER_SIZE] = {0};
        while ((n = read(sock, temp_buffer, sizeof(temp_buffer)-1)) > 0) {
            // Ajouter null-terminator pour strstr
            temp_buffer[n] = '\0';
            
            // Vérifier si on a assez d'espace dans le buffer
            if (total_received + n >= sizeof(complete_buffer)) {
                write(sock, "Erreur: fichier trop grand\n", 27);
                return;
            }
            
            // Copier les nouvelles données
            memcpy(complete_buffer + total_received, temp_buffer, n);
            total_received += n;
            
            // Vérifier si on a reçu le marqueur de fin
            if (strstr(temp_buffer, "END_OF_FILE") != NULL) {
                // Trouver et supprimer le marqueur de fin
                char* end = strstr(complete_buffer, "\nEND_OF_FILE");
                if (end != NULL) {
                    *end = '\0';
                    break;
                }
            }
            
            // Réinitialiser le buffer temporaire
            memset(temp_buffer, 0, sizeof(temp_buffer));
        }

        if (total_received <= 0) {
            write(sock, "Erreur lors de la réception du fichier\n", 39);
            return;
        }

        // Création du nom de fichier horodaté
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char timestamp[20];
        strftime(timestamp, 20, "%Y%m%d_%H%M%S", tm_info);
        
        char new_filepath[BUFFER_SIZE];
        char *dot = strrchr(filepath, '.');
        if (dot != NULL) {
            strncpy(new_filepath, filepath, dot - filepath);
            new_filepath[dot - filepath] = '\0';
            snprintf(new_filepath + strlen(new_filepath), BUFFER_SIZE - strlen(new_filepath), 
                    "_presence_%s.csv", timestamp);
        }

        // Écriture du fichier modifié
        FILE *newfile = fopen(new_filepath, "w");
        if (newfile == NULL) {
            char *error_msg = "Erreur lors de la sauvegarde du fichier\n";
            write(sock, error_msg, strlen(error_msg));
            continue;
        }

        fprintf(newfile, "%s", complete_buffer);
        fclose(newfile);

        printf("[DEBUG] Fichier sauvegardé : %s\n", new_filepath);

        // Confirmation au client et fermeture de la connexion pour ce fichier
        char *success_msg = "Fichier d'appel sauvegardé avec succès.\n";
        write(sock, success_msg, strlen(success_msg));
        break;  // Sortir de la boucle après avoir traité un fichier
    }
}

int unSecured_server(int port) {
    int sockfd, newsockfd;
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    pid_t pid;

    /* First call to socket() function */
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    /* Start listening for the clients */
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            exit(1);
        }

        /* Create child process */
        pid = fork();
        if (pid < 0) {
            perror("ERROR on fork");
            exit(1);
        }

        if (pid == 0) {
            /* This is the client process */
            close(sockfd);
            doprocessing(newsockfd);
            exit(0);
        } else {
            close(newsockfd);
        }
    }
    return 0;
}