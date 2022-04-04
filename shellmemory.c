#include "pcb.h"

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>

/*
[---Variables (e.g. 0 to 99---][----Frame space (e.g. 100 to 999---]
*/

#define VAR_MEM_SIZE 10 //100
#define FRAME_MEM_SIZE 18 //900
#define SHELL_MEM_LENGTH (VAR_MEM_SIZE + FRAME_MEM_SIZE)

struct memory_struct{
	char *var;
	char *value;
};

struct memory_struct shellmemory[SHELL_MEM_LENGTH];

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
	int free_page_index=-1;
	for(int i=VAR_MEM_SIZE; i<SHELL_MEM_LENGTH; i+=3){ //Iterate by PAGES
		if (shellmemory[i].var == "none"){ //If the first index is free, whole page is free
			free_page_index=i;
			break;
		}
	}

	if (free_page_index==-1){
		error_code = 22; ///<-------No memspace error
		return error_code;
	}

	int iterator = 0; //to iterate through file
	int free_index=free_page_index; //index to use for lines
	
	char buf[1000]; //To load the line
  while (fgets(buf,1000, fp)!=NULL) {
    if (iterator == file_startline || iterator == (file_startline+1) || iterator == (file_startline+2)){
			shellmemory[free_index].var = strdup(myPCB->pid);
      shellmemory[free_index].value = strdup(buf);
			free_index++;
    }
  	iterator++;
  }

	//Record in which frame the page is stored
	myPCB->pagetable[page_num]=memindex_to_framenum(free_page_index);

	//Test to print
	// if(page_num==1){
	// 	print_shellmemory();
	// 	print_pagetable(myPCB);
	// }

	//END
	fclose(fp);
	return error_code;
}