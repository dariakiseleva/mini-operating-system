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

#define QUEUE_LENGTH 10
#define MAX_INT 2147483646
PCB* readyQueue[QUEUE_LENGTH];

void ready_queue_initialize()
{
    for (size_t i = 0; i < QUEUE_LENGTH; ++i)
    {
        readyQueue[i] = (PCB*)malloc(sizeof(PCB));
        (*readyQueue[i]).PC = -1;
        (*readyQueue[i]).start = -1;
        (*readyQueue[i]).end = -1;
        (*readyQueue[i]).pid = NULL;
        (*readyQueue[i]).job_length_score = -1;
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
        if ( (*readyQueue[i]).start == -1 ){
            (*readyQueue[i]).PC = (*pPCB).PC;
            (*readyQueue[i]).start = (*pPCB).start;
            (*readyQueue[i]).end = (*pPCB).end;
            (*readyQueue[i]).pid = (*pPCB).pid;
            (*readyQueue[i]).job_length_score = (*pPCB).job_length_score;
            break;
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

int myinit(const char *filename){
    FILE* fp;
    int error_code = 0;
    int* start = (int*)malloc(sizeof(int));
    int* end = (int*)malloc(sizeof(int));
    
    fp = fopen(filename, "rt");
    if(fp == NULL){
        error_code = 11; // 11 is the error code for file does not exist
        return error_code;
    }

    //generate a random ID as file ID
    char* fileID = (char*)malloc(32);
    sprintf(fileID, "%d", rand());

    error_code = add_file_to_mem(fp, start, end, fileID);
    if(error_code != 0){
        fclose(fp);
        return error_code;
    }
    PCB* newPCB = makePCB(*start,*end,fileID);
    newPCB -> job_length_score = 1 + *end - *start;

    ready_queue_add_to_end(newPCB);

    fclose(fp);

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

    //FCFS and SJF: running program will stop when it finishes
    if( policyNumber == 0 || policyNumber == 1 ){
        cpu_quanta_per_program = MAX_INT;
    }else if(policyNumber == 3){
        cpu_quanta_per_program = 1;
    }

    //scheduling logic for 0: FCFS and 2: RR
    if(policyNumber == 0 || policyNumber == 2){
        //keep running programs while ready queue is not empty
        while(ready_queue_pop(0,false).PC != -1)
        {
            PCB firstPCB = ready_queue_pop(0,false);
            load_PCB_TO_CPU(firstPCB.PC);
            
            int error_code_load_PCB_TO_CPU = cpu_run(cpu_quanta_per_program, firstPCB.end);
            
            if(error_code_load_PCB_TO_CPU == 2){
                //the head PCB program has been done, time to reclaim the shell mem
                clean_mem(firstPCB.start, firstPCB.end);
                ready_queue_pop(0,true);
            }
            if(error_code_load_PCB_TO_CPU == 0){
                //the head PCB program has finished its quanta, it need to be put to the end of ready queue
                firstPCB.PC = cpu_get_ip();
                ready_queue_pop(0,true);
                ready_queue_add_to_end(&firstPCB);
            }
        }
    }

    //scheduling policy for 1: SJF
    if(policyNumber == 1){
        while(!is_ready_empty())
        {
            //task with the lowest lines of codes runs first
            int task_index_with_the_least_lines;
            int task_lines = MAX_INT;
            //get the lowest job length 
            for(int i = 0; i < QUEUE_LENGTH; i++){
                if((*readyQueue[i]).start != -1 && 1 + (*readyQueue[i]).end - (*readyQueue[i]).start < task_lines){
                    task_lines = 1 + (*readyQueue[i]).end - (*readyQueue[i]).start;
                    task_index_with_the_least_lines = i;
                }
            }

            PCB current_task_PCB = (*readyQueue[task_index_with_the_least_lines]);
            load_PCB_TO_CPU(current_task_PCB.PC);
            
            int error_code_load_PCB_TO_CPU = cpu_run(cpu_quanta_per_program, current_task_PCB.end);
            
            //the head PCB program has been done, time to reclaim the shell mem
            clean_mem(current_task_PCB.start, current_task_PCB.end);
            //put the current PCB into invalid mode
            terminate_task_in_queue_by_index(task_index_with_the_least_lines);
        }
    }

    //scheduling policy for 3: Aging
    if(policyNumber == 3){
        int task_index_least_job_length_score;
        int task_job_length_score = MAX_INT;

        //find job with the lowest job score
        for(int i = 0; i < QUEUE_LENGTH; i++){
            //get the lowest job length score
            if((*readyQueue[i]).start != -1 && (*readyQueue[i]).job_length_score < task_job_length_score){
                task_job_length_score = (*readyQueue[i]).job_length_score;
                task_index_least_job_length_score = i;
            }
        }
        //move the task with the lowest job score to the front of the queue
        PCB job_with_lowest_job_score = ready_queue_pop(task_index_least_job_length_score,true);
        ready_queue_add_to_front(&job_with_lowest_job_score);
        
        while(!is_ready_empty())
        {
            //task with the lowest job length score runs first
            //in this case, the task with the lowest job length score is the first task in queue
            task_job_length_score = (*readyQueue[0]).job_length_score;
            task_index_least_job_length_score = 0;

            PCB current_task_PCB = (*readyQueue[task_index_least_job_length_score]);
            load_PCB_TO_CPU(current_task_PCB.PC);
            
            int error_code_load_PCB_TO_CPU = cpu_run(cpu_quanta_per_program, current_task_PCB.end);

            if(error_code_load_PCB_TO_CPU == 2){
                //the head PCB program has been done, time to reclaim the shell mem
                clean_mem((*readyQueue[task_index_least_job_length_score]).start, (*readyQueue[task_index_least_job_length_score]).end);
                ready_queue_pop(task_index_least_job_length_score, true);
                task_job_length_score = MAX_INT;
            }

            if(error_code_load_PCB_TO_CPU == 0){
                //the head PCB program has finished its quanta
                (*readyQueue[task_index_least_job_length_score]).PC = cpu_get_ip(); // update the PC for the PCB
                //Age all the tasks (other than the current executing task) in queue by 1
                for(int i = 0; i < QUEUE_LENGTH; i++){
                    //get the lowest job length score
                    if((*readyQueue[i]).start != -1 && (*readyQueue[i]).job_length_score > 0 && i != task_index_least_job_length_score){
                        (*readyQueue[i]).job_length_score -= 1;
                    }
                }
            }
            
            //if the first task job score is not the lowest, 
            //then move the frst task to the end 
            //and the lowest job score task to the front
            for(int i = 0; i < QUEUE_LENGTH; i++){
                //get the lowest job length score
                if((*readyQueue[i]).start != -1 && (*readyQueue[i]).job_length_score < task_job_length_score){
                    task_job_length_score = (*readyQueue[i]).job_length_score;
                    task_index_least_job_length_score = i;
                }
            }
            if(task_index_least_job_length_score != 0){
                //pop the task with the lowest job score 
                PCB lowest_job_score_task = ready_queue_pop(task_index_least_job_length_score, true);
                //move the frst task to the end
                PCB first_pcb = ready_queue_pop(0, true);
                ready_queue_add_to_end(&first_pcb);
                //move the lowest job score task to the front
                ready_queue_add_to_front(&lowest_job_score_task);
            }
        
        }
    }

    //clean up
    ready_queue_Empty();
    cpu_empty();

    return error_code;
}