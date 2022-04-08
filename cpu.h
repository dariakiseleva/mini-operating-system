#ifndef CPU_H
#define CPU_H
#include "pcb.h"

int file_has_page(PCB *myPCB, int page_num);
int cpu_run_virtual(PCB *myPCB);
#endif
