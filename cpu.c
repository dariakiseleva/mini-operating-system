#include "cpu.h"
#include "pcb.h"
#include "interpreter.h"
#include "shell.h"
#include "shellmemory.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//New version: virtual CPU modifies the PCB by reference, calls for each command to execute, and returns error codes

int cpu_run_virtual(PCB *myPCB){
    int errorCode = 0;
    int quanta = 2; //Hard-coded the quanta

    while (quanta!=0){

        //DRAFT-------------
        if (myPCB->pagetable[myPCB->page_counter]==-1){
            errorCode=1;
            return errorCode;
        }

        //Figure out where in memory to look for the command
        int index_in_memory = framenum_to_memindex(myPCB->pagetable[myPCB->page_counter]) + myPCB->line_in_page;

        //Get the command from memory
        char command[1000];
        strncpy(command, mem_get_value_by_line(index_in_memory),1000);

        if ((strcmp(command, "none")==0)){
            errorCode=1; //We stopped in middle of a frame, so this PCB is done
            return errorCode;
        }

        parseInput(command); //-----> Execute the line!

        //Increment where the program advanced for the next CPU operation
        if((myPCB->line_in_page)<2){
            myPCB->line_in_page++;
        } else {
            myPCB->page_counter++;
            myPCB->line_in_page = 0;
        }
        quanta-=1;
    }

    return errorCode;
}