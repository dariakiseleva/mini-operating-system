#include "cpu.h"
#include "pcb.h"
#include "interpreter.h"
#include "shell.h"
#include "shellmemory.h"

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//Return 1 if file still has page we try to access, 0 if not
int file_has_page(PCB *myPCB, int page_num){

    FILE* fp;
        fp = fopen(myPCB->bs_filename, "rt"); 

    int lines = 0;
    char buf[1000]; //To load the line
    while (fgets(buf,1000, fp)!=NULL) {
        if (strcmp(buf, "\n")!=0){
            lines++;
        }
    }

    int last_page = (lines-1)/3;

    // printf("\nLines: %i, Last page: %i\n", lines, last_page);
    if (last_page>=page_num){
        return 1;
    } 
    return 0;
}


//New version: virtual CPU modifies the PCB by reference, calls for each command to execute, and returns error codes

int cpu_run_virtual(PCB *myPCB){

    int errorCode = 0;
    int quanta = 2; //Hard-coded the quanta

    while (quanta!=0){

        //If the next page is not in memory, return PAGE FAULT
        if (file_has_page(myPCB, myPCB->page_counter)==1 && myPCB->pagetable[myPCB->page_counter]==-1){
            errorCode=2;
            return errorCode;
        }

        //Figure out where in memory to look for the command
        int source_frame = myPCB->pagetable[myPCB->page_counter];
        int index_in_memory = framenum_to_memindex(source_frame) + myPCB->line_in_page;

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

        //frame was used, so move it back in the LRU queue
        lru_queue_add_to_end(source_frame);

        quanta-=1;
    }

    return errorCode;
}