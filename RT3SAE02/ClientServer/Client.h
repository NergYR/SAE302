#ifndef CLIENT_H
#define CLIENT_H

#include "server.h"  // Pour BUFFER_SIZE
#include <sys/time.h>

// Définition de la structure Student
typedef struct {
    char etudid[10];
    char nip[10];
    char etat[2];
    char civilite[5];
    char nom[50];
    char nomusuel[50];
    char prenom[50];
    char tp[10];
    char presence;
} Student;

extern void processAttendance(char* buffer, Student* students, int* nbStudents);
extern char* createModifiedCSV(Student* students, int nbStudents);
extern int client_main(int argc, char *argv[]);

#endif
