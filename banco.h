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

const char* dbfile = "banco.txt";
const char* tempfile = "bancotemp.txt";
const char* myfifo = "/tmp/myfifo";
const char* logfile = "log.txt";
const char dbDelimiter = ',';

typedef struct {
    int id;
    char nome[50];
} Registro;

typedef struct {
    Registro reg;
    int command;
    char commandString[9];
} Query;

typedef struct Task {
    char query[QUERY_SIZE];
} Task;

#endif