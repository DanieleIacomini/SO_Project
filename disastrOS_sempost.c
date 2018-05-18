#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
  int fd = running -> syscall_args[0];
  SemDescriptor* des = SemDescriptorList_byFd(&running->sem_descriptors,fd);
  if(!des) {
    running -> syscall_retvalue = DSOS_SEMNOTFD;
  }

  Semaphore* sem = des -> semaphore;
  if(!sem) {
    running -> syscall_retvalue = DSOS_SEMNOTSEM;
  }

  SemDescriptorPtr* des_ptr;
  (sem -> count)++;

  if(sem->count <= 0) {
    List_insert(&ready_list, ready_list.last, (ListItem*) running);
    des_ptr = (SemDescriptorPtr*)List_detach(&sem->waiting_descriptors, (ListItem*) sem->waiting_descriptors.first);
    List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) proc_desptr);
    List_detach(&waiting_list, (ListItem*) proc_desptr->descriptor->pcb);
    running->status = Ready;
    running = proc_desptr->descriptor->pcb;
  }

}
