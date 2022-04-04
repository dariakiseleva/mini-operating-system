#include "pcb.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


PCB* makePCB(char* pid, char* bs_filename) {

    PCB * newPCB = malloc(sizeof(PCB));

    newPCB->pid = pid;

    newPCB->page_counter=0; 
    newPCB->line_in_page=0; 

    newPCB->pagetable[1000]; 
    for (int i=0; i < 1000; i++){
        newPCB->pagetable[i]=-1;
    }

    strcpy(newPCB->bs_filename , bs_filename);

    return newPCB;
}