/*
BRAINSTORMING FUNCTIONS, GENERALLY
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//int add_PAGE_to_mem(FILE* fp, char* fileID, )


int main(int argc, char *argv[]) {

  PCB* newPCB = makePCB("wewer", "backing_store/file1");


  //How to get lines from file
  FILE* fp;
  fp = fopen("prog1", "rt");

  char buf[1000];

  int page_counter = 0;
  int line_in_page = 0;

  int line_in_file = (page_counter*3)+line_in_page;

  int iterator = 0;
  while (fgets(buf,1000, fp)!=NULL) {
    if (iterator == line_in_file){
      printf("%s",buf);
    }
    iterator++;
  }

}

