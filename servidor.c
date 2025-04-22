// Código Template shm_linux_consumer.c do Viel
/**
 * Consumidor - Memória compartilhada - Linux
 * Para compilar
 *	gcc shm-posix-consumer.c -lrt
*/

#include "banco.h"
#define THREAD_NUM 4    //Este é o número de threads que iremos consumir

Task taskQueue[256]; //Fila de tarefas - com no máximo 256 comandos
int taskCount = 0;

pthread_mutex_t mutexBanco; // Usado para proteger o acesso ao banco

pthread_mutex_t mutexLog;

pthread_mutex_t mutexQueue; //protege a fila lá
pthread_cond_t condQueue;  // sinalizar que tem tarefas disponíveis

Query parseQuery(char* query){
    // o \n é inserido pelo banco nos comandos
    query[strcspn(query, "\n")] = 0;

    Query q; 

    q.command = -1;  //váriável para identificar o comando a ser usado

    //Pega o primeiro commando a ser usado
    int lenCommand = strcspn(query," "); //pega o tamanho da substring que possui caracteres até o primeiro espaço

    strncpy(q.commandString, query, lenCommand);
    q.commandString[lenCommand] = '\0';

    printf("%s\n", q.commandString);

    //verifica o tipo de comando
    if(strcmp(q.commandString, "DELETE") == 0) q.command = 0;
    if(strcmp(q.commandString, "INSERT") == 0) q.command = 1;
    if(strcmp(q.commandString, "UPDATE") == 0) q.command = 2;
    if(strcmp(q.commandString, "SELECT") == 0) q.command = 3;
    if(strcmp(q.commandString, "TRUNCATE") == 0) q.command = 4;

    //Procura o id na string e extrai o valor numérico
    char * idFound = strstr(query, "id=");
    if (idFound != NULL) {
        int idPosition = idFound - query + 3; // seguindo https://stackoverflow.com/questions/4824/string-indexof-function-in-c
        char idString[12];
        for(int y=0; y<12; y++){
            if(query[y+idPosition] == ' ' || query[y+idPosition] == '\0') break;
            idString[y] = query[y+idPosition];
        }
        q.reg.id = atoi(idString); // se tiver espaços ele pega o número certinho
    }

    //O mesmo processo só que para o nome
    char * nameFound = strstr(query, "nome=");
    if (nameFound != NULL) {
        int namePosition = nameFound - query + 5; 
        for(int y=0; y<50; y++){
            if(query[y+namePosition] == ' ' || query[y+namePosition] == '\0'){
                q.reg.nome[y] = '\0';
                break;
            }
            q.reg.nome[y] = query[y+namePosition];
        }    
    }
    q.reg.nome[49] = '\0';

    return q;
}

void executeTask(Task* task){
    Query q = parseQuery(task->query); //Faz a interpretação da task

    //para verificar no log
    int thread_id = rand()%100000;
    printf("Começando thread %d\n", thread_id);

    FILE *fptr;
    FILE *fptr2; 
    char currentLine[DB_LINE_SIZE];
    int id;
    bool querySuccess = false;

    switch(q.command){
       case 0: // Delete
        //Lê as informações do banco e copia
            pthread_mutex_lock(&mutexBanco);
            fptr = fopen(dbfile, "r");
            fptr2 = fopen(tempfile, "a");

            //copia todas as linhas exceto o id a seer deletado
            while(fgets(currentLine, DB_LINE_SIZE, fptr)){
                id = atoi(currentLine);
                if(id != q.reg.id){
                    fprintf(fptr2, "%s", currentLine);
                }
                else{
                    querySuccess = true;
                }
            }

            fclose(fptr);
            fclose(fptr2);
            
            //substitui o antigo arquivo pelo novo
            remove(dbfile);
            rename(tempfile, dbfile); 
            pthread_mutex_unlock(&mutexBanco);

       break;
       
       case 1: // Insert
            pthread_mutex_lock(&mutexBanco);
            fptr = fopen(dbfile, "a+");
            querySuccess = true;
            
            //Verifica se o ID já existe
            while(fgets(currentLine, DB_LINE_SIZE, fptr)){
                id = atoi(currentLine);
                if(id == q.reg.id){
                        querySuccess = false;
                }
            }

            //Se não existir, insere o que foi pedido no final do banco
            if(querySuccess) fprintf(fptr, "%d%c%s\n", q.reg.id, dbDelimiter, q.reg.nome);
            
            fclose(fptr);
            pthread_mutex_unlock(&mutexBanco);

       break;
       
       case 2: // Update
            pthread_mutex_lock(&mutexBanco);
            fptr = fopen(dbfile, "r");
            fptr2 = fopen(tempfile, "a");
            
            //Percorre por tudo e reescreve, atualizando apenas o registro com id informado
            while(fgets(currentLine, DB_LINE_SIZE, fptr)){
                id = atoi(currentLine);
                if(id != q.reg.id){
                    fprintf(fptr2, "%s", currentLine);
                }
                else{
                    fprintf(fptr2, "%d%c%s\n", id, dbDelimiter, q.reg.nome);
                    querySuccess = true;
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
            // poderia ser um array de registros para permitir uma consulta maior?
            
            //procura o id e imprime o nome
            while(fgets(currentLine, DB_LINE_SIZE, fptr)){
                id = atoi(currentLine);
                if(id == q.reg.id){ // só buscando pelo id, ainda não tenho certeza como permitir buscar pelo nome também...
                    // pega a primeira ocorrência do delimitar, incrementa por 1 para pegar apenas o nome
                    strcpy(q.reg.nome, strchr(currentLine, dbDelimiter)+1);
                        querySuccess = true;
                    }
            }

            printf("id:%d - nome:%s\n", q.reg.id, q.reg.nome);
            fclose(fptr);
            pthread_mutex_unlock(&mutexBanco);

        break;
        
        case 4: // truncate
            pthread_mutex_lock(&mutexBanco);
            fptr = fopen(dbfile, "w");

            fclose(fptr);
            querySuccess = true;
            pthread_mutex_unlock(&mutexBanco);

        break;

       default: 
           printf("Comando inválido\n");
       break;
    }

    char status[8];
    if(querySuccess) strcpy(status, "Sucesso\0");
    else strcpy(status, "Erro\0");

    q.reg.nome[strcspn(q.reg.nome, "\n")] = 0;
    // para evitar de, quando o nome vem do SELECT (nos outros casos ela já é tratada na função parseQuery), 
    // o caractere '\n' vir junto e deixar o arquivo de log desorganizado

    pthread_mutex_lock(&mutexLog); 
    fptr = fopen(logfile, "a"); //Insere o ocorrido no log

    if(q.command == -1){
        fprintf(fptr, "Erro ao executar, comando inválido\n");
    }
    else{
        fprintf(fptr, "%s ao executar operação %s em id=%d, nome=%s\n", status, q.commandString, q.reg.id, q.reg.nome);
        if(q.command == 3) fprintf(fptr, "SELECT: id=%d, nome=%s\n", q.reg.id, q.reg.nome);    
    }

    fclose(fptr);
    pthread_mutex_unlock(&mutexLog);

    printf("Terminando thread %d\n", thread_id);
}

void submitTask(Task task) {
    pthread_mutex_lock(&mutexQueue);
    taskQueue[taskCount] = task; //adiona a nova task a fila
    taskCount++;  //adiciona mais uma task ao contador
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&condQueue);
}

//Função relacionada ao que a thread irá fazer
void* startThread(void* args) {
    while (1) {
        Task task;

        pthread_mutex_lock(&mutexQueue);
        //Esse loop vai fazer com que a thread fique esperando até a próxima task for adicionada
        while (taskCount == 0) {
            pthread_cond_wait(&condQueue, &mutexQueue);
        }
        
        //Quando for notado que possui uma nova tesk:
        task = taskQueue[0]; //Pega as informações da task
        int i;
        for (i = 0; i < taskCount - 1; i++) {
            taskQueue[i] = taskQueue[i + 1]; //Retira a task da fila
        }
        taskCount--; //Diminui do contador a task retirada
        pthread_mutex_unlock(&mutexQueue);
        executeTask(&task); //Executa com as informações da task retirada
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

    //Cria as threads
    int i;
    for (i = 0; i < THREAD_NUM; i++) {
        if (pthread_create(&th[i], NULL, &startThread, NULL) != 0) {
            perror("Failed to create the thread");
        }
    } 

    while(1){
        //Abre a FIFO enviada pelo cliente
        fd = open(myfifo, O_RDONLY);

        Task t;

        // faz a leitura das informações da FIFO da variável fd
		read(fd, t.query, QUERY_SIZE);

        printf("%s\n", t.query);
        // esse \n é absolutamente necessário senão o print não aparece, quebrei muito a cabeça pra descobrir

        //Adicionar uma task a fila
        submitTask(t);

        //sinaliza que é o fim da escrita da FIFO
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