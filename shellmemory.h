#include<stdlib.h>
#include<stdio.h>
#include "pcb.h"

//Basic memory functions
void mem_init();
char *mem_get_value(char *var);
char* mem_get_value_by_line(int line);
void mem_set_value(char *var, char *value);
int resetmem();

//Functions for demand paging
int has_frame_space();
void clear_frame();
int framenum_to_memindex(int framenum);
int memindex_to_framenum(int memindex);
int load_page(PCB* myPCB, int page_num);

//LRU queue functions
void lru_queue_init();
void lru_queue_add_to_end(int new);
int lru_queue_pop();

//Backing store functions
void reset_backing_store();
void delete_backing_store();