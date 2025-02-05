#ifndef CLIENT_H
#define CLIENT_H

typedef struct {
    char etudid[10];
    char nip[10];
    char etat[10];
    char civilite[10];
    char nom[50];
    char nomusuel[50];
    char prenom[50];
    char tp[10];
    char presence[10];  // Modifier le type de presence
} Student;

extern void processAttendance(char* buffer, Student* students, int* nbStudents);
extern char* createModifiedCSV(Student* students, int nbStudents);
extern int client_main(int argc, char *argv[]);

#endif
