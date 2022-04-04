#include "pcb.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//In this implementation, Pid is the same as file ID 
PCB* makePCB(char* pid, char* bs_filename) {

    PCB * newPCB = malloc(sizeof(PCB));

    //To be deleted when fixed.
    newPCB->start=0;
    newPCB->end=0;
    newPCB->PC=0;
    newPCB->job_length_score=0;

    newPCB->pid = pid;

    newPCB->page_counter=0; ////NEW - Which page of the file is the CPU on
    newPCB->line_in_page=0; ///NEW - 1 to 3, which line within page is the CPU on

    //NOTE: Needs to be initialized to -1s
    newPCB->pagetable[1000]; //NEW - which frame a page is stored. Set to -1 if none.
    for (int i=0; i < 1000; i++){
        newPCB->pagetable[i]=-1;
    }

    strcpy(newPCB->bs_filename , bs_filename);

    return newPCB;
}


//TESTING 

// void testFunction(PCB* newPCB){
//     printf("%s\n", newPCB->bs_filename);
// }

// int main(int argc, char *argv[]) {
//     PCB* newPCB = makePCB("wewer", "backing_store/file1");

//     testFunction(newPCB);

//     printf("%s\n", newPCB->bs_filename);
//     printf("%i\n", newPCB->pagetable[999]);
// }