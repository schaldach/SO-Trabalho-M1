// Código Template shm_linux_consumer.c do Viel
/**
 * Consumidor - Memória compartilhada - Linux
 * Para compilar
 *	gcc shm-posix-consumer.c -lrt
 */

 #include <stdio.h>
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
     int i;

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

     char* c_ptr = (char*)ptr;
     char* command[10] = '';

     i=0;
     while(c_ptr[i] != ' '){
        printf("%c\n", c_ptr[i]);
        command[i] = c_ptr[i];
        i++;
     } 

     printf("%s", command);

     if(strcmp(command, "CREATE") == 0){
        printf("O comando é CREATE");
     }
     if(strcmp(command, "DELETE") == 0){
        printf("O comando é DELETE");
     }

     /* remove the shared memory segment */
     if (shm_unlink(name) == -1) {
         printf("Error removing %s\n",name);
         exit(-1);
     }
 
     return 0;
 }