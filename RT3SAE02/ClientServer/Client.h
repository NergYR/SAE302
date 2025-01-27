#ifndef CLIENT_H
#define CLIENT_H

#include <sys/time.h>

typedef struct {
    char name[256];
    char presence[20];
} Student;

extern void processAttendance(char* buffer, Student* students, int* nbStudents);
extern char* createModifiedCSV(Student* students, int nbStudents);
extern int client_main(int argc, char *argv[]);

#endif
