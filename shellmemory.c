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

//HELPER FUNCTION TO DELETE
void print_shellmemory(){
	for (int i=VAR_MEM_SIZE; i<SHELL_MEM_LENGTH; i++){
		printf("Var: %s, value: %s", shellmemory[i].var, shellmemory[i].value);
	}
}

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


//NEW: Load a page into memory
int load_page(PCB* myPCB, int page_num){

	printf("I will load page %i of file %s\n", page_num, myPCB->bs_filename);

	int error_code = 0;

	//

	return error_code;
}