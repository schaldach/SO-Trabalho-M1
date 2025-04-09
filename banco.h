#ifndef BANCO_H
#define BANCO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdbool.h>
#include <pthread.h>

#define QUERY_SIZE 100
#define DB_LINE_SIZE 70

char* dbfile = "banco.txt";
char* tempfile = "bancotemp.txt";
char * myfifo = "/tmp/myfifo";

typedef struct {
    int id;
    char nome[50];
} Registro;

typedef struct {
    Registro reg;
    int command;
} Query;

typedef struct Task {
    char* query[QUERY_SIZE];
} Task;

#endif