#ifndef PCB_H
#define PCB_H

/*
 * Struct:  PCB 
 * --------------------
 * pid: process(task) id
 * bs_filename: the temporary filename inside the /backing_store directory
 * page_counter: which page (3 lines) in the document the CPU must work on next
 * line_in_page: which line within the page (0, 1, or 2) the CPU must work on next
 * pagetable: array indicating the frame # in shellmemory where a page is stored. E.g. pagetable[0]=1 means that the first page of the file is in frame 1 of shellmemory. If pagetable[x]=-1, then page x is not in shellmemory at all and must be loaded.
 */
typedef struct
{
    char* pid;
    int page_counter; 
    int line_in_page; 
    int pagetable[1000]; 
    char bs_filename[30]; 
}PCB;

PCB * makePCB(char* pid, char* bs_filename);
#endif
