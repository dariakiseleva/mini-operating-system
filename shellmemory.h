#include<stdlib.h>
#include<stdio.h>
#include "pcb.h"

int has_frame_space();
void clear_frame();
void print_shellmemory();
void print_pagetable(PCB* myPCB);
int framenum_to_memindex(int framenum);
int memindex_to_framenum(int memindex);
int load_page(PCB* myPCB, int page_num);
void mem_init();
char *mem_get_value(char *var);
char* mem_get_value_by_line(int line);
void mem_set_value(char *var, char *value);
void clean_mem(int start, int end);
int resetmem();
int add_file_to_mem(FILE* fp, int* pStart, int* pEnd, char* fileID);

void lru_queue_init();
void print_lru_queue();
void lru_queue_add_to_end(int new);
int lru_queue_pop();
