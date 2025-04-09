// Código Template shm_linux_producer.c do Viel
/**
 * Produtor - Memória compartilhada - Linux
 * Para compilar
 * 	gcc shm-posix-producer.c -lrt
 */

 #include "banco.h"
 
 int main(){
    int fd1;
    mkfifo(myfifo, 0666);
    
    char test_string[QUERY_SIZE];
    scanf("%s", test_string);

    fd1 = open(myfifo, O_WRONLY);
    write(fd1, test_string, strlen(test_string)+1);
    // testando com QUERY_SIZE o bug parece que ocorreu mais vezes, mas usando ambos ocorreu.
    close(fd1);

    return 0;
 }