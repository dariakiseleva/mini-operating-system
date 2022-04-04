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

/*
 * Struct:  CPU 
 * --------------------
 * IP: serve as a pointer to shell memory. Ex. IP = 101 means CPU is executing the 101th line in shellmemory
 * IR: stores the line of code that CPU is executing
 * quanta: how many lines of code could the current task run until it finishes or being switched to another task
 */
struct CPU
{
    int IP;
    char IR[1000];
    int quanta;
};
struct CPU aCPU = {.quanta =0};

int cpu_get_ip(){
    return aCPU.IP;
}

void cpu_set_ip(int pIP){
    aCPU.IP = pIP;
}

void cpu_empty(){
    aCPU.quanta = 2;
}

void load_PCB_TO_CPU(int PC){
    cpu_set_ip(PC);
}

/*
 * Function:  cpu_run 
 * 	Added in A2
 * --------------------
 * run "quanta"(first input parameter) lines of code, starting from the cpu.IP
 *
 * quanta: number of lines that the CPU will run before it switches task or the task ends
 * end: the last line of the task in shell memory
 * returns: error code, 2: file reaches an end, 0: no error
 */
int cpu_run(int quanta, int end){
    aCPU.quanta = quanta;
    int error_code;
    while (aCPU.quanta != 0){
        
        strncpy(aCPU.IR, mem_get_value_by_line(aCPU.IP),1000);
        parseInput(aCPU.IR);
        if(end == aCPU.IP){
            error_code = 2;
            return error_code;
        }
        aCPU.IP = aCPU.IP + 1;
        aCPU.quanta -= 1;
    }
    
    error_code = 0;
    return error_code;
}



//////-----------VIRTUAL

int cpu_run_virtual(PCB *myPCB){
    int errorCode = 0;
    int quanta = 6;

    while (quanta!=0){

        //Figure out where in memory to look for the command
        int index_in_memory = framenum_to_memindex(myPCB->pagetable[myPCB->page_counter]) + myPCB->line_in_page;

        //Get the command from memory
        char command[1000];
        strncpy(command, mem_get_value_by_line(index_in_memory),1000);

        if ((strcmp(command, "none")==0)){
            errorCode=1; //-----------> Stopped in middle of frame, PCB is done
            return errorCode;
        }

        parseInput(command);

        //Increment where the program advanced
        if(myPCB->line_in_page<2){
            myPCB->line_in_page++;
        } else {
            myPCB->page_counter++;
            myPCB->line_in_page = 0;
        }
        quanta-=1;
    }

    return errorCode;
}