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

//Helper function to delete
void print_ready_queue(){
    for (int i = 0; i < QUEUE_LENGTH; i++){
        printf("%i: %s", i, readyQueue[i]->pid);
    }
    printf("\n");
}

void ready_queue_initialize()
{
    for (size_t i = 0; i < QUEUE_LENGTH; ++i)
    {
        readyQueue[i] = (PCB*)malloc(sizeof(PCB));
        readyQueue[i]->pid = NULL;        
    }
}

void ready_queue_Empty(){
    for (size_t i = 0; i < QUEUE_LENGTH; ++i)
    {
        (*readyQueue[i]).pid = NULL;
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
            (*readyQueue[i-1])=(*readyQueue[i]);
        }
        (*readyQueue[QUEUE_LENGTH-1]).pid = NULL;
    }
    return head;
}

void ready_queue_add_to_end(PCB *pPCB)
{
    for(int i = 0; i < QUEUE_LENGTH; i++){
        if (readyQueue[i]->pid == NULL ){

            (*readyQueue[i])=(*pPCB);
            break;
        }
    }
}

void ready_queue_add_to_front(PCB *pPCB){
    for (size_t i = QUEUE_LENGTH-1; i > 0; i--){
        (*readyQueue[i])=(*readyQueue[i-1]);
    }
    (*readyQueue[0])=(*pPCB);
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






// Only left part of scheduler that deals with RR policy
int scheduler(int policyNumber){
    int error_code = 0;

    //keep running programs while ready queue is not empty
    while(ready_queue_pop(0,false).pid != NULL)
    {
        //Get the first PCB in the queue and send it to the CPU
        PCB firstPCB = ready_queue_pop(0,false);
        int cpu_error = cpu_run_virtual(&firstPCB);

        //If PCB is done, take it off the queue
        if(cpu_error == 1){
            ready_queue_pop(0,true); //Pop the head of the queue with removal
        }

        //If PAGE FAULT
        if(cpu_error == 2){
            ready_queue_pop(0,true); //Put PCB in back of queue

            //Take some page out of memory -> print the message to screen, update the pagetable for that other PCB

            //Load one more page of the original PCB, update its pagetable

            //Erase frame contents
            char* kicked_pid = clear_frame();



            printf("\nThe kicked pid was: %s\n", kicked_pid);
            exit(1);
        }

        if(cpu_error == 0){
            //the head PCB program has finished its quanta, it need to be put to the end of ready queue
            ready_queue_pop(0,true);
            ready_queue_add_to_end(&firstPCB);
        }
    }

    //clean up
    ready_queue_Empty();

    return error_code;
}