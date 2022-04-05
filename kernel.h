#ifndef kernel_H
#define kernel_H

#include "pcb.h"

//Variable
extern int file_num;
extern PCB* readyQueue[];

//Functions
PCB* get_PCB_from_pid(char* pid);
void ready_queue_initialize();
int get_scheduling_policy_number(char* policy);
int myinit(const char *filename);
int scheduler();

#endif
