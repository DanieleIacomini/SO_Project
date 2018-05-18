#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "disastrOS_globals.h"     //aggiungo il file globals.h per le variabili globali , tra le quali semaphores_list
#include "disastrOS_constants.h"	//aggiungo il file constants.h per i messaggi di errore
#include "linked_list.h"				//per la List_Detach


void internal_semClose(){
	  int check;									//Variabile per controllo errori
	  int fd = running->syscall_args[0];    		//Argomento sem_close

	  SemDescriptor* sem_desc = SemDescriptorList_byFd(&semaphores_list, fd);
		 //Cerco il SemDescriptor tramite il suo fd
	  if(!sem_desc){
			running -> syscall_retvalue = DSOS_SEMNOTFD;
	  }



	  Semaphore* sem = sem_desc-> semaphore; 			//Prendo il semaforo corrispondente al descrittore Sem_Desc
	  if(!sem){
			running -> syscall_retvalue = DSOS_SEMNOTSEM;
	  }

	  SemDescriptorPtr* sem_descptr = sem_desc->ptr;   //Riferimento
	  if(!sem_descptr){
			running -> syscall_retvalue = DSOS_SEMNOTDESPTR;
	  }



	  List_detach(&running->sem_descriptors, (ListItem*) sem_desc); //Elimino il descrittore dalla lista del processo

	  check = SemDescriptor_free(sem_desc);						//Rilascio le risorse del Sem_Descriptor
	  if(!check){
			running -> syscall_retvalue = DSOS_SEMNOTFREE;
	  }

 List_detach(&sem->descriptors, (ListItem*) sem_descptr);
		//Elimino dalla lista dei descrittori del semaforo il riferimento a SemDescriptorPtr

	  check = SemDescriptorPtr_free(sem_descptr);				//Rilascio le risorse del Sem_DescriptorPTR
	  if(!check) {
			running -> syscall_retvalue = DSOS_SEMNOTFREE;
	  }

	  if((sem->descriptors).size ==  0){ //!! && (sem->waiting_descriptors).size == 0){ DA RICONTROLLARE
		   List_detach(&semaphores_list,(ListItem*)sem);
			check = Semaphore_free(sem);						//Rilascio le risorse del Semaforo solo se non ci sono più descrittori associati
			 if(!check){
				 running -> syscall_retvalue = DSOS_SEMNOTFREE;
				}
	  }


    running->syscall_retvalue = 0;    //RESTITUISCO 0 nella syscall_retvalue
	return;
}
