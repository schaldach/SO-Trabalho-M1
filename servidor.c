// Código Template shm_linux_consumer.c do Viel
/**
 * Consumidor - Memória compartilhada - Linux
 * Para compilar
 *	gcc shm-posix-consumer.c -lrt
*/

#include "banco.h"
#define THREAD_NUM 4

Task taskQueue[256];
int taskCount = 0;

pthread_mutex_t mutexBanco;

pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

Query parseQuery(char* query){
    // (documentação no cliente)
    // o \n é inserido pelo banco nos comandos
    query[strcspn(query, "\n")] = 0;

    int command = -1;
    char queryCommand[7];
    for(int y=0;y<6;y++) queryCommand[y] = query[y];
    queryCommand[6] = '\0'; // senão vem caracteres estranhos no final, as vezes
    printf("%s\n", queryCommand);

    if(strcmp(queryCommand, "DELETE") == 0) command = 0;
    if(strcmp(queryCommand, "INSERT") == 0) command = 1;
    if(strcmp(queryCommand, "UPDATE") == 0) command = 2;
    if(strcmp(queryCommand, "SELECT") == 0) command = 3;

    int id;
    char * idFound = strstr(query, "id=");
    if (idFound != NULL) {
        int idPosition = idFound - query + 3; // seguindo https://stackoverflow.com/questions/4824/string-indexof-function-in-c
        char idString[12];
        for(int y=0; y<12; y++){
            if(query[y+idPosition] == ' ' || query[y+idPosition] == '\0') break;
            idString[y] = query[y+idPosition];
        }
        id = atoi(idString); // se tiver espaços ele pega o número certinho
    }

    char nameString[50];
    char * nameFound = strstr(query, "nome=");
    if (nameFound != NULL) {
        int namePosition = nameFound - query + 5; 
        for(int y=0; y<50; y++){
            if(query[y+namePosition] == ' ' || query[y+namePosition] == '\0'){
                nameString[y] = '\0';
                break;
            }
            nameString[y] = query[y+namePosition];
        }    
    }
    nameString[49] = '\0';

    Query q; 
    q.reg.id = id;
    strcpy(q.reg.nome, nameString);
    q.command = command;

    return q;
}

void executeTask(Task* task){
    Query q = parseQuery(task->query);

    int thread_id = rand()%100000;
    printf("Começando thread %d\n", thread_id);

    FILE *fptr;
    FILE *fptr2; 
    char currentLine[DB_LINE_SIZE];
    int id;

    switch(q.command){
       case 0: // Delete
           pthread_mutex_lock(&mutexBanco);
           fptr = fopen(dbfile, "r");
           fptr2 = fopen(tempfile, "a");

           while(fgets(currentLine, DB_LINE_SIZE, fptr)){
               id = atoi(currentLine);
               if(id != q.reg.id){
                   fprintf(fptr2, "%s", currentLine);
               }
           }

           fclose(fptr);
           fclose(fptr2);
       
           remove(dbfile);
           rename(tempfile, dbfile); 
           pthread_mutex_unlock(&mutexBanco);

       break;
       
       case 1: // Insert
           pthread_mutex_lock(&mutexBanco);
           fptr = fopen(dbfile, "a+");
           bool id_exists = false;

           while(fgets(currentLine, DB_LINE_SIZE, fptr)){
               id = atoi(currentLine);
               if(id == q.reg.id){
                   id_exists = true;
               }
           }

           if(!id_exists) fprintf(fptr, "%d,%s\n", q.reg.id, q.reg.nome);
           else printf("id já existe\n");
           
           fclose(fptr);
           pthread_mutex_unlock(&mutexBanco);

       break;
       
       case 2: // Update
           pthread_mutex_lock(&mutexBanco);
           fptr = fopen(dbfile, "r");
           fptr2 = fopen(tempfile, "a");

           while(fgets(currentLine, DB_LINE_SIZE, fptr)){
               id = atoi(currentLine);
               if(id != q.reg.id){
                   fprintf(fptr2, "%s", currentLine);
               }
               else{
                   fprintf(fptr2, "%d,%s\n", id, q.reg.nome);
               }
           }

           fclose(fptr);
           fclose(fptr2);
   
           remove(dbfile);
           rename(tempfile, dbfile); 
           pthread_mutex_unlock(&mutexBanco);

       break;
       
       case 3: // Select
           pthread_mutex_lock(&mutexBanco);
           fptr = fopen(dbfile, "r");
           Registro reg; // poderia ser um array para permitir uma consulta maior?

           while(fgets(currentLine, DB_LINE_SIZE, fptr)){
               id = atoi(currentLine);
               if(id == q.reg.id){ // só buscando pelo id, ainda não tenho certeza como permitir buscar pelo nome também...
                   reg.id = id;

                   bool is_on_str = false;
                   int str_position = 0;
                   for(int i=0;i<DB_LINE_SIZE;i++){
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

           printf("id:%d - nome:%s\n", reg.id, reg.nome);
           fclose(fptr);
           pthread_mutex_unlock(&mutexBanco);

       break;

       default: 
           printf("Comando inválido\n");
       break;
    }

    printf("Terminando thread %d\n", thread_id);
}

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

int main(){
    int fd;
    mkfifo(myfifo, 0666);
 
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_mutex_init(&mutexBanco, NULL);

    pthread_cond_init(&condQueue, NULL);

    // um comportamento muito bizarro é que as vezes parece que o comando chega mais de uma vez
    // mas não da pra perceber isso de outra forma senão olhando pro log,
    // porque realmente nao interfere em nada
    // muito esquisto, mas ok

    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(&th[i], NULL, &startThread, NULL) != 0) {
            perror("Failed to create the thread");
        }
    } 

    while(1){
        fd = open(myfifo, O_RDONLY);

        Task t;

		read(fd, t.query, QUERY_SIZE);

        printf("%s\n", t.query);
        // esse \n é absolutamente necessário senão o print não aparece, quebrei muito a cabeça pra descobrir

        submitTask(t);

		close(fd);
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