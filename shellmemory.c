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
Memory is a single array with the structure:
[---Variables (var_mem_size)---][----Frames (frame_mem_size, which is a multiple of 3)----]
*/

//Key constants
#define SHELL_MEM_LENGTH (VAR_MEM_SIZE + FRAME_MEM_SIZE)
#define HIGHEST_FRAME_INDEX ((FRAME_MEM_SIZE/3)-1)
#define lru_length (FRAME_MEM_SIZE/3)

//Key objects
struct memory_struct{
	char *var;
	char *value;
};
struct memory_struct shellmemory[SHELL_MEM_LENGTH];
int lru_queue[lru_length];


///----------BASIC MEMORY FUNCTIONS

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

//get value by line in shell memory
char* mem_get_value_by_line(int line){
	return shellmemory[line].value;
}

//New command: Delete contents of variable store only
int resetmem(){
	  for(int i = 0; i < VAR_MEM_SIZE; i ++){
      shellmemory[i].var = "none";
			shellmemory[i].value = "none";
    }
}


//-----------FUNCTIONS FOR DEMAND PAGING

//See if variable store has space for one more frame, return 1 if yes and 0 if no
int has_frame_space(){
	for(int i=VAR_MEM_SIZE; i<SHELL_MEM_LENGTH; i+=3){
		if (strcmp(shellmemory[i].var, "none")==0){
			return 1; //Yes, has space
		}
	}
	return 0; //No, has no space
}

//Helper functions for conversions between # of frame (starting at 0) and the index in the shellmemory array
int framenum_to_memindex(int framenum){
	int memindex = VAR_MEM_SIZE + (framenum * 3);
	return memindex;
}

int memindex_to_framenum(int memindex){
	int framenum = (memindex - VAR_MEM_SIZE)/3;
	return framenum;
}

//Load a page into memory: takes the PCB and which page of that file should be loaded (pages start at 0)
int load_page(PCB* myPCB, int page_num){

	int error_code = 0;
	int file_startline = page_num*3;

	//Open the corresponding file in the backing store
	FILE* fp;
	fp = fopen(myPCB->bs_filename, "rt");

	//Make sure file exists, otherwise return appropriate error
	if(fp == NULL){
		error_code = 11; 
		return error_code;
	}

	//Find a free frame in memory, store its first index
	int free_frame_index=-1;
	for(int i=VAR_MEM_SIZE; i<SHELL_MEM_LENGTH; i+=3){ //Iterate by PAGES
		if (shellmemory[i].var == "none"){ //If the first index is free, whole page is free
			free_frame_index=i;
			break;
		}
	}

	//If no free frame in memory, return appropriate error
	if (free_frame_index==-1){
		error_code = 21; 
		return error_code;
	}

	int iterator = 0; //to iterate through file
	int free_index=free_frame_index; //index to use for lines
	
	//Load up to three lines from file, starting at index found above, into shellmemory
	char buf[1000];
  while (fgets(buf,1000, fp)!=NULL) {
    if (iterator == file_startline || iterator == (file_startline+1) || iterator == (file_startline+2)){
			shellmemory[free_index].var = strdup(myPCB->pid);
      shellmemory[free_index].value = strdup(buf);
			free_index++;
    }
  	iterator++;
  }

	//Record in which frame the page is stored in the pagetable of the PCB
	int free_frame_num = memindex_to_framenum(free_frame_index);
	myPCB->pagetable[page_num]=free_frame_num;

	//Page was used, so move it to the back of the LRU queue
	lru_queue_add_to_end(free_frame_num);

	//Close file and return 0 if successful
	fclose(fp);
	return error_code;
}

//Empty one frame in memory, update pagetable of the PCB to which the page belonged
void clear_frame(){

	//The frame that must be cleared is the least recently used one - index 0 in the LRU queue
	//It is an integer 0 or above
	int frame_to_remove = lru_queue_pop(); 

	//Search shellmemory to find the pid of the PCB which had the code in the frame to be emptied
	char *victim_pid = shellmemory[framenum_to_memindex(frame_to_remove)].var;

	//Print to screen the content kicked out of shellmemory
	//Immediately after, set the shellmemory slots to empty values
	printf("Page fault! Victim page contents:\n");
	int line_num = 0;
	while(line_num < 3){
		if (strcmp(shellmemory[framenum_to_memindex(frame_to_remove)+line_num].var, "none")!=0){
			printf("%s", shellmemory[framenum_to_memindex(frame_to_remove)+line_num].value);
			shellmemory[framenum_to_memindex(frame_to_remove)+line_num].var="none";
			shellmemory[framenum_to_memindex(frame_to_remove)+line_num].value="none";
		}
		line_num ++;
	}
	printf("End of victim page contents.\n");

	//Use the pid to find which PCB had its code taken out of memory
	PCB* kicked_PCB = get_PCB_from_pid(victim_pid);

	//Update the pagetable of the kicked PCB to indicate that one of its pages is no longer in memory
	for (int i=0; i < 1000; i++){
		if(kicked_PCB->pagetable[i]==frame_to_remove){
			kicked_PCB->pagetable[i]=-1;
		}
	}

	//frame was used, so move it to the back of the LRU queue
  lru_queue_add_to_end(frame_to_remove);

}


//-----------LRU QUEUE FUNCTIONS

/* The LRU queue is an array of ints which are the frame #s starting at 0. The frame in index 0 of the LRU queue is the one that was least recently used etc.
*/


//Initialize LRU queue
void lru_queue_init(){
    for (int i = 0; i < lru_length; i++)
    {
        lru_queue[i] = -1;
    }
}


//Add OR move a frame number to the back of the queue
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
}

//Remove first element from queue and return it
int lru_queue_pop(){
  int first = lru_queue[0];
  for (int i=0; i<lru_length-1; i++){
    lru_queue[i]=lru_queue[i+1];
  }
  lru_queue[lru_length-1]=-1;
  return first;
}


//---------BACKING STORE FUNCTIONS

//Ensure that an empty folder "backing_store" exists in the current directory
void reset_backing_store(){
  struct stat st = {0};
  //If Directory doesn't exist, create it
  if (stat("./backing_store", &st) == -1) {
      system("mkdir backing_store");
  } 
  //If directory exists, delete it first and then call create again
  else {
    delete_backing_store();
    reset_backing_store();
  }
}

//Delete the "backing_store" folder
void delete_backing_store(){
  system("rm -rf  backing_store");
}