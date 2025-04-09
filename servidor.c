// Código Template shm_linux_consumer.c do Viel
/**
 * Consumidor - Memória compartilhada - Linux
 * Para compilar
 *	gcc shm-posix-consumer.c -lrt
*/

#include "banco.h"
#define THREAD_NUM 4

typedef struct Task {
    char* query[100];
} Task;
Task taskQueue[256];
int taskCount = 0;

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

void submitTask(Task task) {
    pthread_mutex_lock(&mutexQueue);
    taskQueue[taskCount] = task;
    taskCount++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}

void* startThread(void* args) {
    while (1) {
        Task task;

        pthread_mutex_lock(&mutexQueue);
        while (taskCount == 0) {
            pthread_cond_wait(&condQueue, &mutexQueue);
        }

        task = taskQueue[0];
        int i;
        for (i = 0; i < taskCount - 1; i++) {
            taskQueue[i] = taskQueue[i + 1];
        }
        taskCount--;
        pthread_mutex_unlock(&mutexQueue);
        executeTask(&task);
    }
}

void executeCommand(Task* task){
    FILE *fptr;
    FILE *fptr2; 
    char currentLine[70];
    int id;

    switch(command){
       case 0: // Delete
           fptr = fopen(dbfile, "r");
           fptr2 = fopen(tempfile, "a");

           while(fgets(currentLine, 70, fptr)){
               id = atoi(currentLine);
               if(id != new_id){
                   fprintf(fptr2, "%s", currentLine);
               }
           }

           fclose(fptr);
           fclose(fptr2);
       
           remove(dbfile);
           rename(tempfile, dbfile); 
       break;
       
       case 1: // Insert
           // checar se id existe
           fptr = fopen(dbfile, "a+");
           bool id_exists = false;

           while(fgets(currentLine, 70, fptr)){
               id = atoi(currentLine);
               if(id == new_id){
                   id_exists = true;
               }
           }

           if(!id_exists) fprintf(fptr, "%d,%s", new_id, new_string);
           else printf("id já existe");
           
           fclose(fptr);
       break;
       
       case 2: // Update
           fptr = fopen(dbfile, "r");
           fptr2 = fopen(tempfile, "a");

           while(fgets(currentLine, 70, fptr)){
               id = atoi(currentLine);
               if(id != new_id){
                   fprintf(fptr2, "%s", currentLine);
               }
               else{
                   fprintf(fptr2, "%d,%s", id, new_string);
               }
           }

           fclose(fptr);
           fclose(fptr2);
   
           remove(dbfile);
           rename(tempfile, dbfile); 
       break;
       
       case 3: // Select
           fptr = fopen(dbfile, "r");
           Registro reg; // poderia ser um array para permitir uma consulta maior?

           while(fgets(currentLine, 70, fptr)){
               id = atoi(currentLine);
               if(id == new_id){ // só buscando pelo id, ainda não tenho certeza como permitir buscar pelo nome também...
                   reg.id = id;

                   bool is_on_str = false;
                   int str_position = 0;
                   for(int i=0;i<70;i++){
                       if(is_on_str){
                           reg.nome[i-str_position] = currentLine[i];
                       }
                       else if(currentLine[i] == ','){
                           is_on_str = true;
                           str_position = i+1;
                       }
                   }
               }
           }

           printf("id:%d - nome:%s", reg.id, reg.nome);

           fclose(fptr);
       break;

       default: 
           printf("Comando inválido");
       break;
    }
}

int main(){
     int shm_fd;
     void *ptr;

     shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
     ftruncate(shm_fd,SIZE);
     if (shm_fd == -1) {
        printf("shared memory failed\n");
        exit(-1);
     }
     ptr = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
     if (ptr == MAP_FAILED) {
         printf("Map failed\n");
         exit(-1);
     }
 
     pthread_t th[THREAD_NUM];
     pthread_mutex_init(&mutexQueue, NULL);
     pthread_cond_init(&condQueue, NULL);
     int i;
     for (i = 0; i < THREAD_NUM; i++) {
         if (pthread_create(&th[i], NULL, &startThread, NULL) != 0) {
             perror("Failed to create the thread");
         }
     } 

     /* now read from the shared memory region */
     while(1){
        char* c_ptr = (char*)ptr;
        printf("%s", c_ptr);
     }

     // Query* c_ptr = (Query*)ptr;
     char* c_ptr = (char*)ptr;
     int command = 3;
     int new_id = 2;
     char* new_string = "novo_nomedsadsa";
     
     /* remove the shared memory segment */
     if (shm_unlink(name) == -1) {
         printf("Error removing %s\n",name);
         exit(-1);
     }
 
     for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join the thread");
        }
    }
    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);

     return 0;
 }