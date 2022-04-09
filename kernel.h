#ifndef kernel_H
#define kernel_H

#include "pcb.h"
#include <stdbool.h>

//Variables to be used in other files
extern int file_num;
extern PCB* readyQueue[];

//Ready queue functions
void ready_queue_initialize();
void ready_queue_Empty();
void ready_queue_destory();
PCB ready_queue_pop(int index, bool inPlace);
void ready_queue_add_to_end(PCB *pPCB);

//Helper functions
PCB* get_PCB_from_pid(char* pid);
int get_scheduling_policy_number(char* policy);

//KEY FUNCTIONS
int myinit(const char *filename);
int scheduler();

#endif
