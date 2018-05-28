#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>

#include "disastrOS_globals.h" //Mi serve per la lista dei semafori

#include "disastrOS.h"

#define BUFFER_SIZE 4
#define MAX_TRANSACTION     1000
#define CICLES 5


int num[BUFFER_SIZE];


int read_index,write_index;
//int sem; da capire se é utile
int total;



static inline int performRandomTransaction() {

    int amount = rand() % (2 * MAX_TRANSACTION); // {0, ..., 2*MAX_TRANSACTION - 1}
    if (amount++ >= MAX_TRANSACTION) {
        // now amount is in {MAX_TRANSACTION + 1, ..., 2*MAX_TRANSACTION}
        return MAX_TRANSACTION - amount; // {-MAX_TRANSACTION, ..., -1}
    } else {
        // now amount is a number between 1 and MAX_TRANSACTION
        return amount;
    }
}


void producer(int prod,int cons){

    for (int i = 0; i < CICLES; i++){


	   int numero = performRandomTransaction();
	   printf("-----STO PER FARE SEMWAIT(prod)-----\n");
        disastrOS_semwait(prod);
        //disastrOS_semwait(sem);

        printf("Controllo prima di produrre dopo le wait\n\n");

       // printf("semaforo prod sta a ---> %d", prod->count); NON FUNZIONA VA CREATO UN SEMAFORO APPOSITO PER STAMPARE
         disastrOS_printStatus();

        num[write_index] = numero;
        write_index = (write_index + 1) % BUFFER_SIZE;

        printf("-----PRODUCED-----\n");
         printf("NewWriteindex %d\n",write_index);
        printf("Il processo %d", disastrOS_getpid());
        printf(" ha prodotto la risorsa %d\n\n",numero);



        //disastrOS_sempost(sem);
        printf("-----STO PER FARE SEMPOST(cons)-----\n");
        disastrOS_sempost(cons);

        printf("Controllo dopo la post dopo aver prodotto\n\n");
         disastrOS_printStatus();

}

}

void consumer(int prod,int cons){

	//disastrOS_sleep(20);

    for (int i = 0; i < CICLES; i++){
		printf("-----STO PER FARE SEMWAIT(cons)-----\n");
        disastrOS_semwait(cons);
        //disastrOS_semwait(sem);

         printf("Controllo prima di consumare la risorsa dopo la wait\n\n");

         disastrOS_printStatus();

        int lastnum = num[read_index];
        total+=lastnum;
        num[read_index] = 0;
         printf("Il processo che sta per consumare la risorsa ha pid %d\n", disastrOS_getpid());
        read_index = (read_index + 1) % BUFFER_SIZE;
         printf("NewReadIndex %d\n",read_index);


         printf("-----CONSUMED-----\n");
        printf("La somma totale delle operazioni è: %d.\n\n", total);


        //disastrOS_sempost(sem);
        printf("-------STO PER FARE SEMPOST(prod)-------\n");
        disastrOS_sempost(prod);

         printf("Controllo dopo la post dopo aver consumato la risorsa\n\n");
        disastrOS_printStatus();




    }
}


// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    //disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");
  int type=0;
  int mode=0;
  int fd=disastrOS_openResource(disastrOS_getpid(),type,mode);
  printf("fd=%d\n", fd);



  //!Parte aggiunta da noi !


  read_index  = 0;
  write_index = 0;

  int cons = disastrOS_semopen(1,0);


  int prod = disastrOS_semopen(2,BUFFER_SIZE);

  //sem = disastrOS_semopen(3,1);

  //disastrOS_printStatus();


   printf("-----WAITING SOME SECS BEFORE PRODUCING-----\n\n\n");
  disastrOS_sleep(10);



  if (disastrOS_getpid() == 2) {
      printf("-----PROCESS N 2 WILL PRODUCE-----\n");
      disastrOS_sleep(10);
      producer(prod, cons);
  }

  if (disastrOS_getpid() == 3){
    printf("-----PROCESS N 3 WILL CONSUME-----\n");
    consumer(prod, cons);
  }




  //disastrOS_printStatus();

   printf("PID: %d, terminating\n", disastrOS_getpid());
   disastrOS_printStatus();


  //disastrOS_semclose(sem);
printf("-----CLOSING SEMAPHORES-----\n\n\n\n");

  disastrOS_semclose(cons);
  disastrOS_semclose(prod);

  disastrOS_printStatus();



  //! Fine della parte aggiunta da noi !





/*
  for (int i=0; i<(disastrOS_getpid()+1); ++i){
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*5);
  }*/

  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  //disastrOS_printStatus();
  printf("hello, I am init and I just started\n");

  disastrOS_spawn(sleeperFunction, 0);

  printf("I feel like to spawn 4 nice threads\n");
  int alive_children=0;
  for (int i=0; i<2; ++i) { 	//DIMINUITO NUM. PROCESSI
    //int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    //int fd=disastrOS_openResource(i,type,mode);
    //printf("fd=%d\n", fd);
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }

  //disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){
    //disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }
/*int i;
  for (i=0; i<BUFFER_SIZE; ++i) {
        printf("closing resource %d\n",fd);
        disastrOS_closeResource(fd);
        disastrOS_destroyResource(i);
    }*/

  disastrOS_printStatus();
  printf("SHUTDOWN!\n");
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);



  return 0;
}
