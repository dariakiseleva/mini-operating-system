#include<stdlib.h>
#include<stdio.h>
#include "pcb.h"

int pagenum_to_memindex(int pagenum);
int memindex_to_pagenum(int memindex);
int load_page(PCB* myPCB, int page_num);
void mem_init();
char *mem_get_value(char *var);
char* mem_get_value_by_line(int line);
void mem_set_value(char *var, char *value);
void clean_mem(int start, int end);
int resetmem();
int add_file_to_mem(FILE* fp, int* pStart, int* pEnd, char* fileID);
