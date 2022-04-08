#include "pcb.h"
#include "kernel.h"
#include "shellmemory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*

Frame Store Size = 21; Variable Store Size = 10

[---Variables---][----Frames (multiple of 3)----]

*/

#define SHELL_MEM_LENGTH (VAR_MEM_SIZE + FRAME_MEM_SIZE)
#define HIGHEST_FRAME_INDEX ((FRAME_MEM_SIZE/3)-1)
#define lru_length (FRAME_MEM_SIZE/3)

struct memory_struct{
	char *var;
	char *value;
};

struct memory_struct shellmemory[SHELL_MEM_LENGTH];
int lru_queue[lru_length];

//Print before greeting

//HELPER FUNCTION TO DELETE LATER
void print_shellmemory(){
	printf("\n");
	for (int i=VAR_MEM_SIZE; i<SHELL_MEM_LENGTH; i++){
			if (shellmemory[i].var!="none"){
			printf("Index: %i, Var: %s, value: %s", i, shellmemory[i].var, shellmemory[i].value);
		}
	}
}

void print_pagetable(PCB* myPCB){

	printf("\n\nPagetable of PID %s\n\n", myPCB->pid);
	for (int i=0; i<1000; i++){
		if (myPCB->pagetable[i]!=-1){
			printf("pagetable[%i]=%i\n", i, myPCB->pagetable[i]);
		}
	}
}


///-------------


// Shell memory functions

void mem_init(){

	int i;
	for (i=0; i<SHELL_MEM_LENGTH; i++){		
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

// Set key value pair
void mem_set_value(char *var_in, char *value_in) {

	int i;

	for (i=0; i<SHELL_MEM_LENGTH; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=0; i<SHELL_MEM_LENGTH; i++){
		if (strcmp(shellmemory[i].var, "none") == 0){
			shellmemory[i].var = strdup(var_in);
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	return;

}

//get value based on input key
char *mem_get_value(char *var_in) {
	int i;

	for (i=0; i<SHELL_MEM_LENGTH; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			return strdup(shellmemory[i].value);
		} 
	}
	return "Variable does not exist";

}

char* mem_get_value_by_line(int line){
	return shellmemory[line].value;
}


void clean_mem(int start, int end){
    for(int i = start; i <= end; i ++){
      shellmemory[i].var = "none";
			shellmemory[i].value = "none";
    }
}


//Delete contents of variable store
int resetmem(){
	  for(int i = 0; i < VAR_MEM_SIZE; i ++){
      shellmemory[i].var = "none";
			shellmemory[i].value = "none";
    }
}


int has_frame_space(){
	for(int i=VAR_MEM_SIZE; i<SHELL_MEM_LENGTH; i+=3){
		if (strcmp(shellmemory[i].var, "none")==0){
			return 1; //Yes, has space
		}
	}
	return 0; //No, has no space
}

//Helper functions for conversions
int framenum_to_memindex(int framenum){
	int memindex = VAR_MEM_SIZE + (framenum * 3);
	return memindex;
}

int memindex_to_framenum(int memindex){
	int framenum = (memindex - VAR_MEM_SIZE)/3;
	return framenum;
}

//Load a page into memory
int load_page(PCB* myPCB, int page_num){

	int error_code = 0;
	int file_startline = page_num*3;

	FILE* fp;
	fp = fopen(myPCB->bs_filename, "rt"); //Take new file, not original

	//Make sure file exists
	if(fp == NULL){
		error_code = 11; // 11 is the error code for file does not exist
		return error_code;
	}

	//Find a free page in memory, store its first index
	int free_frame_index=-1;
	for(int i=VAR_MEM_SIZE; i<SHELL_MEM_LENGTH; i+=3){ //Iterate by PAGES
		if (shellmemory[i].var == "none"){ //If the first index is free, whole page is free
			free_frame_index=i;
			break;
		}
	}

	if (free_frame_index==-1){
		error_code = 21; ///<-------No memspace error
		return error_code;
	}

	int iterator = 0; //to iterate through file
	int free_index=free_frame_index; //index to use for lines
	int loaded_something = 0;
	
	char buf[1000]; //To load the line
  while (fgets(buf,1000, fp)!=NULL) {
    if (iterator == file_startline || iterator == (file_startline+1) || iterator == (file_startline+2)){
			loaded_something = 1;
			shellmemory[free_index].var = strdup(myPCB->pid);
      shellmemory[free_index].value = strdup(buf);
			free_index++;
    }
  	iterator++;
  }

	//If nothing was loaded, the file has finished
	//Return specific error
	if (loaded_something==0){
		printf("\nNothing to load, %s does not have page #%i\n", myPCB->pid, page_num);
		exit(1);
	}

	//Record in which frame the page is stored
	int free_frame_num = memindex_to_framenum(free_frame_index);
	myPCB->pagetable[page_num]=free_frame_num;

	//Page was used, so move it back in the LRU queue
	lru_queue_add_to_end(free_frame_num);
	//print_lru_queue();

	//END
	fclose(fp);
	return error_code;
}


//Choose a page to take out of memory, update pagetable of the PCB to which the page belonged
void clear_frame(){

	//Choose frame to remove - for now randomly
	int frame_to_remove = (rand() % (HIGHEST_FRAME_INDEX - 0 + 1)) + 0; 
	frame_to_remove = lru_queue_pop(); //TRY POPPING!

	//Record which program/PCB the victim frame was occupied by
	char *victim_pid = shellmemory[framenum_to_memindex(frame_to_remove)].var;


	printf("Page fault! Victim page contents:\n");
	//Iterate through up to 3 lines of the frame, print them out and clear them
	int line_num = 0;
	while(line_num < 3){
		if (strcmp(shellmemory[framenum_to_memindex(frame_to_remove)+line_num].var, "none")!=0){
			printf("%s", shellmemory[framenum_to_memindex(frame_to_remove)+line_num].value);
			shellmemory[framenum_to_memindex(frame_to_remove)+line_num].var="none";
			shellmemory[framenum_to_memindex(frame_to_remove)+line_num].value="none";
		}
		line_num ++;
	}
	printf("End of victim page content.\n");

	//Find which PCB had its page erased
	PCB* kicked_PCB = get_PCB_from_pid(victim_pid);


	//Update the pagetable of the kicked PCB
	for (int i=0; i < 1000; i++){
		if(kicked_PCB->pagetable[i]==frame_to_remove){
			kicked_PCB->pagetable[i]=-1;
		}
	}

	//frame was used, so move it back in the LRU queue
  lru_queue_add_to_end(frame_to_remove);

}

//-----------LRU QUEUE

//Initialize LRU queue
void lru_queue_init(){
    for (int i = 0; i < lru_length; i++)
    {
        lru_queue[i] = -1;
    }
}

//Helper function!
void print_lru_queue(){
  for (int i=0; i<lru_length; i++){
     printf("lru[%i]: %i\n", i, lru_queue[i]);
  }
}

//Just add to end, or if already in queue -move to end
void lru_queue_add_to_end(int new){

  int existing_index = -1;
  //find if it is in queue
  for (int i=0; i<lru_length; i++){
    if (lru_queue[i]==new){
      existing_index=i;
    }
  }
  //If it is in queue, shift the rest
  if(existing_index!=-1){
    for (int i=existing_index; i<lru_length-1; i++){
      lru_queue[i]=lru_queue[i+1];
    }
    lru_queue[lru_length-1]=-1;
  }

  //add to the end
  for (int i=0; i<lru_length; i++){
    if (lru_queue[i]==-1){
      lru_queue[i]=new;
      break;
    }
  } 

	//print_lru_queue();
}

//Remove first element from queue, return it
int lru_queue_pop(){
  int first = lru_queue[0];
  for (int i=0; i<lru_length-1; i++){
    lru_queue[i]=lru_queue[i+1];
  }
  lru_queue[lru_length-1]=-1;
  return first;
}


//---------BACKING STORE

void reset_backing_store(){
  struct stat st = {0};
  //If Directory doesn't exist, we create it
  if (stat("./backing_store", &st) == -1) {
      system("mkdir backing_store");
  } 
  //If it exists, we delete it first and then call create again
  else {
    delete_backing_store();
    reset_backing_store();
  }
}

void delete_backing_store(){
  system("rm -rf  backing_store");
}