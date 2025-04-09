// Código Template shm_linux_producer.c do Viel
/**
 * Produtor - Memória compartilhada - Linux
 * Para compilar
 * 	gcc shm-posix-producer.c -lrt
 */

 #include "banco.h"
 
 int main(){
    int fd;
    mkfifo(myfifo, 0666);
    char* test_string = "SELECT tanana WHERE tanana";
    fd = open(myfifo, O_WRONLY);
    write(fd, test_string, QUERY_SIZE+1);
    close(fd);

    return 0;
 }