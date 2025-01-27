#ifndef CLIENT_H
#define CLIENT_H

#include <sys/time.h>

typedef struct {
    char name[256];
    char presence[20];
} Student;

void processAttendance(char* buffer, Student* students, int* nbStudents);
char* createModifiedCSV(Student* students, int nbStudents);

#endif
