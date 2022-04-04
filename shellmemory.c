#include "pcb.h"

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>

/*
[---Variables (e.g. 0 to 99---][----Frame space (e.g. 100 to 999---]
*/

#define VAR_MEM_SIZE 100
#define FRAME_MEM_SIZE 900
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


/*
 * Function:  addFileToMem 
 * 	Added in A2
 * --------------------
 * Load the source code of the file fp into the shell memory:
 * 		Loading format - var stores fileID, value stores a line
 *		Note that the first [[[100]]] lines are for set command, the rests are for run and exec command
 *
 *  pStart: This function will store the first line of the loaded file 
 * 			in shell memory in here
 *	pEnd: This function will store the last line of the loaded file 
 			in shell memory in here
 *  fileID: Input that need to provide when calling the function, 
 			stores the ID of the file
 * 
 * returns: error code, 21: no space left
 */
int add_file_to_mem(FILE* fp, int* pStart, int* pEnd, char* fileID)
{
    char line[SHELL_MEM_LENGTH];
    size_t i;
		size_t j;
    int error_code = 0;
	bool hasSpaceLeft = false;

    for (i = VAR_MEM_SIZE; i < SHELL_MEM_LENGTH; i++){
        if(strcmp(shellmemory[i].var,"none") == 0){
            *pStart = (int)i;
			hasSpaceLeft = true;
            break;
        }
    }

	//shell memory is full
	if(hasSpaceLeft == 0){
		error_code = 21;
		return error_code;
	}
    
    for (j = i; j < SHELL_MEM_LENGTH; j++){
        if(feof(fp))
        {
            *pEnd = (int)j-1;
            break;
        }else{
            fgets(line, 999, fp);
						shellmemory[j].var = strdup(fileID);
            shellmemory[j].value = strdup(line);
        }
    }

	//no space left to load the entire file into shell memory
	if(!feof(fp)){
		error_code = 21;
		//clean up the file in memory
		//printf(shellmemory);
		// for(int k = i; k < j; k ++){
		// 	shellmemory[k].var = "none";
		// 	shellmemory[k].value = "none";
    // 	}
		return error_code;
	}

    return error_code;
}



//Helper functions for conversions
int pagenum_to_memindex(int pagenum){
	int memindex = VAR_MEM_SIZE + (pagenum * 3);
	return memindex;
}

int memindex_to_pagenum(int memindex){
	int pagenum = (memindex - VAR_MEM_SIZE)/3;
	return pagenum;
}

//NEW: Load a page into memory
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

	//free_page_index
	myPCB->pagetable[page_num]=memindex_to_pagenum(free_page_index);
	////

	//Test to print
	if(page_num==1){
		print_shellmemory();
		print_pagetable(myPCB);
	}

	//END
	fclose(fp);
	return error_code;
}