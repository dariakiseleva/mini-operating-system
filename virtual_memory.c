#include "virtual_memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


void create_backing_store(){
  struct stat st = {0};
  //If Directory doesn't exist, we create it
  if (stat("./backing_store", &st) == -1) {
      system("mkdir backing_store");
     // mkdir("./backing_store", 0700);
  } 
  //If it exists, we delete it first and then call create again
  else {
    delete_backing_store();
    create_backing_store();
  }
}

void delete_backing_store(){
  system("rm -rf  backing_store");
}


//CODE FOR TEXTING
int main(int argc, char *argv[]) {
  // delete_backing_store();
  // create_backing_store();
}