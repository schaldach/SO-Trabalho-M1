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
    fgets(test_string, QUERY_SIZE, stdin);

    // a diferença entre fgets e scanf é que fgets retorna o '\n' também, o que
    // é ruim, pois o '\n' já é adicionado no banco (necessário para se a string passar do limite)
    // e queremos o banco organizado, então vamos tirar o '\n'
    // (isso foi movido para o lado do servidor, para deixar o cliente mais livre)
    printf("%s\n", test_string);

    fd1 = open(myfifo, O_WRONLY);
    write(fd1, test_string, strlen(test_string)+1);

    close(fd1);

    return 0;
 }