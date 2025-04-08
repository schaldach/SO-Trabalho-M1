// Código Template shm_linux_producer.c do Viel
/**
 * Produtor - Memória compartilhada - Linux
 * Para compilar
 * 	gcc shm-posix-producer.c -lrt
 */

 #include "banco.h"
 
 int main(){
     const char *message0= "dóisahdosad";
     const char *message1= "bla=1";
     const char *message2= "das=2";
 
     int shm_fd;
     void *ptr;
 
     /* create the shared memory segment */
     shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
 
     /* configure the size of the shared memory segment */
     ftruncate(shm_fd,SIZE);
 
     /* now map the shared memory segment in the address space of the process */
     ptr = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
     if (ptr == MAP_FAILED) {
         printf("Map failed\n");
         return -1;
     }
 
     /**
      * Now write to the shared memory region.
       *
      * Note we must increment the value of ptr after each write.
      */
     sprintf(ptr,"%s",message0);
     ptr += strlen(message0);
     sprintf(ptr,"%s",message1);
     ptr += strlen(message1);
     sprintf(ptr,"%s",message2);
     ptr += strlen(message2);
 
     return 0;
 }