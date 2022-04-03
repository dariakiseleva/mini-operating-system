#ifndef kernel_H
#define kernel_H

#include "pcb.h"

//Variable
extern int file_num;

//Functions
void ready_queue_initialize();
int get_scheduling_policy_number(char* policy);
int myinit(const char *filename);
int scheduler();

#endif
