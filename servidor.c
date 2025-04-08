// Código Template shm_linux_consumer.c do Viel
/**
 * Consumidor - Memória compartilhada - Linux
 * Para compilar
 *	gcc shm-posix-consumer.c -lrt
 */

 #include "banco.h"

 int main(){
     int shm_fd;
     void *ptr;

     /* open the shared memory segment */
     shm_fd = shm_open(name, O_RDONLY, 0666);
     if (shm_fd == -1) {
         printf("shared memory failed\n");
         exit(-1);
     }
 
     /* now map the shared memory segment in the address space of the process */
     ptr = mmap(0,SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
     // acho que para permitir escrita pelo servidor ao cliente (SELECT) teríamos que mudar essas permissões, "PROT_READ" e "O_RDONLY"
     if (ptr == MAP_FAILED) {
         printf("Map failed\n");
         exit(-1);
     }
 
     /* now read from the shared memory region */

     // Query* c_ptr = (Query*)ptr;
     char* c_ptr = (char*)ptr;
     int command = 3;
     int new_id = 2;
     char* new_string = "novo_nomedsadsa";

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
                        if(currentLine[i] == ','){
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
     
     /* remove the shared memory segment */
     if (shm_unlink(name) == -1) {
         printf("Error removing %s\n",name);
         exit(-1);
     }
 
     return 0;
 }