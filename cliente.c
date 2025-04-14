// Código Template shm_linux_producer.c do Viel
/**
 * Produtor - Memória compartilhada - Linux
 * Para compilar
 * 	gcc shm-posix-producer.c -lrt
 */

 #include "banco.h"
 
 int main(){
   int fd1;
   char test_string[QUERY_SIZE];
   mkfifo(myfifo, 0666);

   while(1){
      fd1 = -1;
      fgets(test_string, QUERY_SIZE, stdin);

      // a diferença entre fgets e scanf é que fgets retorna o '\n' também, o que
      // é ruim, pois o '\n' já é adicionado no banco (necessário para se a string passar do limite)
      // e queremos o banco organizado, então vamos tirar o '\n'
      // (isso foi movido para o lado do servidor, para deixar o cliente mais livre)
      
      //Verifica se foi inserido algum valor
      if(strcmp(test_string,"\n") == 0){
         printf("[Error] Please enter a command:\n");
         
      }else if(strcmp(test_string,"sair\n") == 0){ //caso foi inserido "sair", o cliente será finalizado
         break;
      }
      else{ // Executar a atividade
         printf("\"%s\"\n", test_string);

         fd1 = open(myfifo, O_WRONLY);
         write(fd1, test_string, strlen(test_string)+1);
         
         close(fd1);
      
      }
         
   }

   return 0;
 }