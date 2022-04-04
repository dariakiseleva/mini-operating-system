#ifndef PCB_H
#define PCB_H

/*
 * Struct:  PCB 
 * --------------------
 * pid: process(task) id
 * PC: program counter, stores the line that the task is executing
 * start: the first line in shell memory that belongs to this task
 * end: the last line in shell memory that belongs to this task
 * job_length_score: for EXEC AGING use only, stores the job length score
 */
typedef struct
{

    //To be deleted
    int start;
    int end;
    int PC;
    int job_length_score;


    char* pid;
    int page_counter; ////NEW - Which page of the file is the CPU on
    int line_in_page; ///NEW - 1 to 3, which line within page is the CPU on
    int pagetable[1000]; //NEW - which frame a page is stored. Set to -1 if none?
    char bs_filename[30]; //NEW
}PCB;

PCB * makePCB(char* pid, char* bs_filename);
#endif
