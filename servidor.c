// Código Template shm_linux_consumer.c do Viel
/**
 * Consumidor - Memória compartilhada - Linux
 * Para compilar
 *	gcc shm-posix-consumer.c -lrt
 */

 #include <stdlib.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include <sys/shm.h>
 #include <sys/stat.h>
 #include <sys/mman.h>
 #include <string.h>
 #include <stdio.h>

 int main(){
     const char *name = "OS";
     const int SIZE = 4096;
 
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
     if (ptr == MAP_FAILED) {
         printf("Map failed\n");
         exit(-1);
     }
 
     /* now read from the shared memory region */

     // query_struct* c_ptr = (query_struct*)ptr;
     char* c_ptr = (char*)ptr;
     int command = 3;

     FILE *fptr;
     fptr = fopen("banco.txt", "w");

     switch(command){
        case 0: // Delete
        // fgets(myString, 100, fptr)
        break;
        
        case 1: // Insert
         
        break;
        
        case 2: // Update
         
        break;
        
        case 3: // Select
          
        break;

        default: 
            printf("Comando inválido");
        break;
     }
     
     fclose(fptr);

     /* remove the shared memory segment */
     if (shm_unlink(name) == -1) {
         printf("Error removing %s\n",name);
         exit(-1);
     }
 
     return 0;
 }