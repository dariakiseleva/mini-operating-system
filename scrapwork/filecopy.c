#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int file_num = 1; ///NOTE: This must be reset!

void test_filecopy(const char *filename){

  //store name of new file, which is: file1, file2, or file3
  char newfile[20]; 
  sprintf(newfile, "backing_store/file%i", file_num);

  //write the system command and run it
  char command[50];
  sprintf(command, "cp %s %s", filename, newfile);
  system(command);

  //increment global file_num
  file_num++;


  //TEST OPENING
  FILE* fp;
  fp = fopen(newfile, "rt"); //Take new file, not original
  if(fp == NULL){
    printf("ERROR");
  } else {
    printf("IT WORKED");
  }

}

int main(int argc, char *argv[]) {
  test_filecopy("prog1");
  test_filecopy("prog1");
}