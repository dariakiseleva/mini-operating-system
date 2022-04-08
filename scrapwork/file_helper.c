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

#include <math.h>


int helper(char* address, int page){

  FILE* fp;
	fp = fopen("./backing_store/file1", "rt"); 

  int lines = 0;
  char buf[1000]; //To load the line
  while (fgets(buf,1000, fp)!=NULL) {
    if (strcmp(buf, "\n")!=0){
      lines++;
    }
  }

  lines = 0;
  int last_page = (lines-1)/3;

  printf("%i lines, pages: %i\n", lines, last_page);

  
  // int lines = 1;
  // char ch;
	// FILE* fp;
	// fp = fopen("./backing_store/file1", "rt"); 
  // while(!feof(fp)) {
  //   ch = fgetc(fp);
  //   if(ch == '\n')
  //   {
  //     lines++;
  //   }
  // }

  // int example_page = page;
  // int last_page = ceil(lines/3);

  // printf("\nLines: %i, last page: %i\n", lines, last_page);

  // if (last_page>=example_page){
  //   return 1;
  // } else {
  //   return 0;
  // }
  return 0;
}


int main(int argc, char *argv[]) {

  printf("%i", helper("./backing_store/file1", 6));


}

