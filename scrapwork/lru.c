
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>




#define lru_length 7
int lru_queue[lru_length] = {11, 2, 3, 4, 5, 6, 7};


// void lru_queue_initialize(){
//     for (size_t i = 0; i < QUEUE_LENGTH; ++i)
//     {
//         readyQueue[i] = (PCB*)malloc(sizeof(PCB));
//         readyQueue[i]->pid = NULL;        
//     }
// }


void lru_queue_init(){
    for (int i = 0; i < lru_length; i++)
    {
        lru_queue[i] = -1;
    }
}

void print_lru_queue(){
  for (int i=0; i<lru_length; i++){
     printf("lru[%i]: %i\n", i, lru_queue[i]);
  }
}

void lru_queue_add_to_end(int new){

  int existing_index = -1;
  //find if it is in queue
  for (int i=0; i<lru_length; i++){
    if (lru_queue[i]==new){
      existing_index=i;
    }
  }

  printf("%i Existing index\n", existing_index);
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

int lru_queue_pop(){
  int first = lru_queue[0];
  for (int i=0; i<lru_length-1; i++){
    lru_queue[i]=lru_queue[i+1];
  }
  lru_queue[lru_length-1]=-1;
  return first;
}

int main(int argc, char *argv[]) {
  lru_queue_init();
  lru_queue_add_to_end(0);
  lru_queue_add_to_end(1);
  lru_queue_add_to_end(2);
  lru_queue_add_to_end(1);


  //int first = lru_queue_pop();
  //lru_queue_add_to_end(first);

  print_lru_queue();
  //printf("%i", first);
}