#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

#include "linked_list.h"			//aggiungo il file linked_list.h per le funzioni Insert e Detach
#include "disastrOS_globals.h"     //aggiungo il file globals.h per le variabili globali , tra le quali semaphores_list
#include "disastrOS_constants.h"	//aggiungo il file constants.h per i messaggi di errore

void internal_semWait(){
  int fd = running -> syscall_args[0];
  SemDescriptor* des = SemDescriptorList_byFd(&running -> sem_descriptors,fd);
  if(!des) {
    running -> syscall_retvalue = /*ERROR MESSAGE*/
    return;
  }

  SemDescriptorPtr* descptr = des -> ptr; //prende Sem_descriptorPtr dal sem_descriptors
  if(!descptr) {
    running -> syscall_retvalue = /*ERROR MESSAGE*/
    return;
  }

  Semaphore* sem = des -> semaphore; //prende semaforo dal SemDescriptor
  if(!sem) {
    running -> syscall_retvalue = /*ERROR MESSAGE*/
    return;
  }

  PCB* p;
  sem -> count = (sem -> count -1); //decrementa semaforo (wait)
  if(sem -> count < 0) {
    List_detach(&sem -> descriptors,(ListItem*)descptr);
    List_insert(&sem -> waiting_descriptors, sem -> waiting_descriptors.last,(ListItem*)des->ptr);
    List_insert(&waiting_list,waiting waiting_list.last,(ListItem*)ready_list.first);
    running -> status = Waiting;  //controllare
    p = (PCB*)List_detach(&ready_list,(ListItem*)ready_list.first);
    running = (PCB*) p;
  }

  running -> syscall_retvalue = 0; /*si ritorna sempre 0 */
  return;
}
