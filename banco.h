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

char* dbfile = "banco.txt";
char* tempfile = "bancotemp.txt";

typedef struct {
    int id;
    char nome[50];
} Registro;

typedef struct {
    Registro reg;
    // ...
} Query;

const int SIZE = 4096;
const char *name = "OS";

#endif