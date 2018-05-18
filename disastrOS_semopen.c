#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"


#include "disastrOS_globals.h"     //aggiungo il file globals.h per le variabili globali , tra le quali semaphores_list
#include "disastrOS_constants.h"	//aggiungo il file constants.h per i messaggi di errore

void internal_semOpen(){

  int id = running->syscall_args[0];    		//Primo Argomento sem_open
  int value = running->syscall_args[1]; 		//Secondo Argomento sem_open


  Semaphore* sem = SemaphoreList_byId(&semaphores_list, id);  	//Cerco il semaforo in semaphores_list con il suo ID.

  if(!sem){
	  sem = Semaphore_alloc(id, value);   			//Se il semaforo non esiste in semaphores_list, lo alloco.
	  }
	  if(!sem){
      running -> syscall_retvalue = DSOS_SEMNOTALLOC;
		  return;
		  }

  if (value < 0){   			//Semaforo con valore minore di zero
    	 running->syscall_retvalue = DSOS_SEMWRONGVALUE;
  }

  List_insert(&semaphores_list, semaphores_list.last, (ListItem*) sem);   //Aggiungo il semaforo a semaphores_list



	int fd = running->last_sem_fd;   //Ultimo sem_fd che si può usare

	SemDescriptor* sem_desc = SemDescriptor_alloc(fd, sem, running);  //Alloco il SemDescriptor che si rifersce a sem
	if(!sem_desc){
    running->syscall_retvalue = DSOS_SEMNOTALLOC;
		return;
	}
	
	List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*) sem_desc);
    //Aggiungo il descrittore allocato alla lista dei descrittori Sem_descriptors

	running->last_sem_fd++;  //Aggiorno il valore di last_sem_fd per il prossimo semaforo.

	



	SemDescriptorPtr* sem_descptr = SemDescriptorPtr_alloc(sem_desc);  //Alloco il SemDescriptorPtr per le risorse
	if(!sem_descptr){
		running -> syscall_retvalue = DSOS_SEMNOTDESPTR;
		return;
					}

	sem_desc->ptr = sem_descptr;  //Aggiungo al ptr di sem_desc il sem_desptr appena allocato


	List_insert(&sem->descriptors, sem->descriptors.last,(ListItem*) sem_descptr);  //Aggiungo alla ListHead Descriptors di sem il descrittore ptr.



	running->syscall_retvalue = sem_desc->fd;   //Restituisco l' fd del descrittore sem_desc che si riferisce al nostro semaforo.
	return;

}
