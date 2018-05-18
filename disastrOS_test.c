#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>

#include "disastrOS.h"

#define BUFFER_SIZE 10
#define MAX_TRANSACTION     1000

int transactions[BUFFER_SIZE];


int read_index,write_index;
int sem;

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
   // for (int i = 0; i < CICLES; i++){
   while(1){
	   
	   int numero = performRandomTransaction();
	   
        disastrOS_semwait(prod);
        disastrOS_semwait(sem);
        
        transactions[write_index] = numero;
        write_index = (write_index + 1) % BUFFER_SIZE;
        printf("PRODUCED\n");
        
        disastrOS_sempost(sem);
        disastrOS_sempost(cons);
    }
}

void consumer(int prod,int cons){
    //for (int i = 0; i < CICLES; i++){
    while(1){
        disastrOS_semwait(cons);
        disastrOS_semwait(sem);
        
        //int lastTransaction = transactions[read_index];
        //transactions[read_index] = 0
        
        read_index = (read_index + 1) % BUFFER_SIZE;
        printf("CONSUMED\n");
        
        disastrOS_sempost(sem);
        disastrOS_sempost(prod);
    }
}


// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
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
  
  sem = disastrOS_semopen(3,1);
  
  disastrOS_printStatus();
  
  /*
   printf("********WAITING SOME SECS BEFORE PRODUCING********\n");
  disastrOS_sleep(20);
  
  
  
  if (disastrOS_getpid() == 3) {
      printf("********PROCESS N 3 WILL PRODUCE AND PROCESS N 4 WILL CONSUME********\n");
      producer(prod, cons);
  }

  if (disastrOS_getpid() == 4){
      consumer(prod, cons);
  }
  * 
  * */
  printf("PID: %d, terminating\n", disastrOS_getpid());

  
  
  //disastrOS_printStatus();
  
   printf("PID: %d, terminating\n", disastrOS_getpid());
  
  printf("********CLOSING SEMAPHORES********\n");
  
  disastrOS_semclose(prod);
  disastrOS_semclose(cons);
  
  
  
  //! Fine della parte aggiunta da noi !
  
 
/*
  for (int i=0; i<(disastrOS_getpid()+1); ++i){
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*5);
  }*/
  
  disastrOS_exit(disastrOS_getpid()+1);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
 
  disastrOS_spawn(sleeperFunction, 0);
  
  printf("I feel like to spawn 4 nice threads\n");
  int alive_children=0;
  for (int i=0; i<4; ++i) { 	//DIMINUITO NUM. PROCESSI
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    int fd=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd);
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }
  
  disastrOS_printStatus();
  printf("shutdown!");
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
  
  
  disastrOS_semclose(sem);
  return 0;
}
