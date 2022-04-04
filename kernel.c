#include "pcb.h"
#include "kernel.h"
#include "cpu.h"
#include "shell.h"
#include "shellmemory.h"
#include "interpreter.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

//Key variable to keep track of filenames, this must be reset!
int file_num = 1; ///NOTE: This must be reset!

#define QUEUE_LENGTH 10
#define MAX_INT 2147483646
PCB* readyQueue[QUEUE_LENGTH];

void ready_queue_initialize()
{
    for (size_t i = 0; i < QUEUE_LENGTH; ++i)
    {
        readyQueue[i] = (PCB*)malloc(sizeof(PCB));
        readyQueue[i]->pid = NULL;
        

        // //DELETE
        // (*readyQueue[i]).PC = -1;
        // (*readyQueue[i]).start = -1;
        // (*readyQueue[i]).end = -1;
        // (*readyQueue[i]).job_length_score = -1;
        // //

        // (*readyQueue[i]).pid = NULL;
        // (*readyQueue[i]).bs_filename = NULL;


        // (*readyQueue[i]).pagetable = buff_table;
        
    }
}

void ready_queue_Empty(){
    for (size_t i = 0; i < QUEUE_LENGTH; ++i)
    {
        (*readyQueue[i]).PC = -1;
        (*readyQueue[i]).start = -1;
        (*readyQueue[i]).end = -1;
        (*readyQueue[i]).pid = NULL;
        (*readyQueue[i]).job_length_score = -1;
    }
}

void ready_queue_destory()
{
    for (size_t i = 0; i < QUEUE_LENGTH; ++i)
    {
        free(readyQueue[i]);
    }
}

PCB ready_queue_pop(int index, bool inPlace)
{
    PCB head = (*readyQueue[index]);
    if(inPlace){
        for (size_t i = index+1; i < QUEUE_LENGTH; i++){
            (*readyQueue[i-1]).PC = (*readyQueue[i]).PC;
            (*readyQueue[i-1]).start = (*readyQueue[i]).start;
            (*readyQueue[i-1]).end = (*readyQueue[i]).end;
            (*readyQueue[i-1]).pid = (*readyQueue[i]).pid;
            (*readyQueue[i-1]).job_length_score = (*readyQueue[i]).job_length_score;
        }
        (*readyQueue[QUEUE_LENGTH-1]).PC = -1;
        (*readyQueue[QUEUE_LENGTH-1]).start = -1;
        (*readyQueue[QUEUE_LENGTH-1]).end = -1;
        (*readyQueue[QUEUE_LENGTH-1]).pid = NULL;
        (*readyQueue[QUEUE_LENGTH-1]).job_length_score = -1;
    }
    return head;
}

void ready_queue_add_to_end(PCB *pPCB)
{
    for(int i = 0; i < QUEUE_LENGTH; i++){
        if (readyQueue[i]->pid == NULL ){

            //printf("We got to here");

            (*readyQueue[i])=(*pPCB);

            // printf("\n\nThis is the pid at the end of the queue: %s\n\n", readyQueue[i]->pid);
            break;

            // //To be deleted
            // (*readyQueue[i]).PC = (*pPCB).PC;
            // (*readyQueue[i]).start = (*pPCB).start;
            // (*readyQueue[i]).end = (*pPCB).end;
            // (*readyQueue[i]).job_length_score = (*pPCB).job_length_score;
            // //To be deleted

            // (*readyQueue[i]).pid = (*pPCB).pid;
            
            // //char buff[100];
            // strcpy((*readyQueue[i]).bs_filename , (*pPCB).bs_filename);

            // //(*readyQueue[i]).bs_filename = (*pPCB).bs_filename;
            // (*readyQueue[i]).page_counter = (*pPCB).page_counter;
            // (*readyQueue[i]).line_in_page = (*pPCB).line_in_page;

            // int buff_table[1000];
            // for (int i=0; i<1000; i++){
            //     buff_table[i]=(*pPCB).pagetable[i];
            // }

            // (*readyQueue[i]).pagetable = buff_table;
        }
    }
}

void ready_queue_add_to_front(PCB *pPCB){
    for (size_t i = QUEUE_LENGTH-1; i > 0; i--){
        (*readyQueue[i]).PC = (*readyQueue[i-1]).PC;
        (*readyQueue[i]).start = (*readyQueue[i-1]).start;
        (*readyQueue[i]).end = (*readyQueue[i-1]).end;
        (*readyQueue[i]).pid = (*readyQueue[i-1]).pid;
        (*readyQueue[i]).job_length_score = (*readyQueue[i-1]).job_length_score;
    }
    // readyQueue[0] = pPCB;
    (*readyQueue[0]).PC = (*pPCB).PC;
    (*readyQueue[0]).start = (*pPCB).start;
    (*readyQueue[0]).end = (*pPCB).end;
    (*readyQueue[0]).pid = (*pPCB).pid;
    (*readyQueue[0]).job_length_score = (*pPCB).job_length_score;
}

bool is_ready_empty(){
    for (size_t i = 0; i < QUEUE_LENGTH; ++i)
    {
        if((*readyQueue[i]).start != -1){
            return false;
        }  
    }
    return true;
}

void terminate_task_in_queue_by_index(int i){
    (*readyQueue[i]).start = -1; 
    (*readyQueue[i]).end = -1; 
    (*readyQueue[i]).PC = -1; 
    (*readyQueue[i]).pid = NULL;
    (*readyQueue[i]).job_length_score = -1;
}

///-------------------NEW MYINIT

int myinit(const char*filename){

    int error_code = 0;

    //Create a name for this file in backing_store
    char bs_filename[20]; 
    sprintf(bs_filename, "backing_store/file%i", file_num);

    //Copy the file into the backing store with a new name
    char command[50];
    sprintf(command, "cp %s %s", filename, bs_filename);
    system(command);

    //increment global file_num
    file_num++;

    //generate a random ID as file ID
    char* fileID = (char*)malloc(32);
    sprintf(fileID, "%d", rand());

    //Create PCB
    PCB* newPCB = makePCB(fileID, bs_filename);

    //Load two pages, return error if something is wrong? For now?
    for (int page_num=0; page_num<=1; page_num++){
        error_code = load_page(newPCB, page_num); //---> Send to load page
        if(error_code != 0){
            //fclose(fp);
            return error_code;
        }
    }

    //FIX THIS LATER
    ready_queue_add_to_end(newPCB);

    return error_code;

}



int get_scheduling_policy_number(char* policy){
    if(strcmp("FCFS",policy)==0){
        return 0;
    }else if(strcmp("SJF",policy)==0){
        return 1;
    }else if(strcmp("RR",policy)==0){
        return 2;
    }else if(strcmp("AGING",policy)==0){
        return 3;
    }else{
        //error code 15
        return 15;
    }
}


/*
 * Variable:  schedulingPolicy 
 * --------------------
 * 0: FCFS
 * 1: SJF
 * 2: RR
 * 3: AGING
 */
int scheduler(int policyNumber){
    int error_code = 0;

    int cpu_quanta_per_program = 2;

    //keep running programs while ready queue is not empty
    while(ready_queue_pop(0,false).pid != NULL)
    {
        PCB firstPCB = ready_queue_pop(0,false);

        int cpu_error = cpu_run_virtual(&firstPCB);
        printf("\nI will work on the PCB with wht PID: %s", firstPCB.pid);


        exit(1);
        
        // if(error_code_load_PCB_TO_CPU == 2){
        //     //the head PCB program has been done, time to reclaim the shell mem
        //     //clean_mem(firstPCB.start, firstPCB.end); //Delete this, don't clean memory
        //     ready_queue_pop(0,true);
        // }
        // if(error_code_load_PCB_TO_CPU == 0){
        //     //the head PCB program has finished its quanta, it need to be put to the end of ready queue
        //     firstPCB.PC = cpu_get_ip(); //FIX THIS
        //     ready_queue_pop(0,true);
        //     ready_queue_add_to_end(&firstPCB);
        // }
    
    }

    //clean up
    ready_queue_Empty();
    cpu_empty();

    return error_code;
}