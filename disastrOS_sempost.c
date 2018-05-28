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

  //SemDescriptorPtr* des_ptr;
  (sem -> count)++;

  if(sem->count <= 0) {
    /*List_insert(&ready_list, ready_list.last, (ListItem*) running);
    des_ptr = (SemDescriptorPtr*)List_detach(&sem->waiting_descriptors, (ListItem*) sem->waiting_descriptors.first);
    List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) des_ptr);
    List_detach(&waiting_list, (ListItem*) des_ptr->descriptor->pcb);
    running->status = Ready;
    running =des_ptr->descriptor->pcb;
    */
    SemDescriptorPtr* head_wait_descriptor = (SemDescriptorPtr*) List_detach(&(sem->waiting_descriptors), (ListItem*) (sem->waiting_descriptors).first);

        PCB* pcb_head = head_wait_descriptor->descriptor->pcb;              //prendo il pcb del processo appena preso
        List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*) head_wait_descriptor);
        List_detach(&waiting_list, (ListItem*) pcb_head);                               //lo rimuovo dalla lista di attesa...
        List_insert(&ready_list, (ListItem*) ready_list.last, (ListItem*) pcb_head);    //...e lo inserisco in quello dei processi ready

        pcb_head->status = Ready;                                           //cambio lo status del processo


        int ret = SemDescriptorPtr_free (head_wait_descriptor);             //dealloco il puntatore al descrittore del sem in attesa
        if(ret) {
            running->syscall_retvalue = ret;
            return;
        }

  }

   running->syscall_retvalue = 0;
  return;
}
