#ifndef CPU_H
#define CPU_H
#include "pcb.h"

int cpu_run_virtual(PCB *myPCB);
int cpu_get_ip();
void cpu_empty();
void load_PCB_TO_CPU(int PC);
int cpu_run(int quanta, int end);
#endif
