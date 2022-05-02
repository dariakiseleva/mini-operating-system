# Summary

The mini OS can execute simple commands, such as storing values in variables and printing words or variable values. Commands can be given in interactive mode, or programs are simulated by files with lines of commands in batch mode. In batch mode, programs are assigned PCBs and scheduled with different policies like First-Come-First-Serve or Round Robin.

Simulated physical memory has frames and initially two pages of each file are loaded. A disk is simulated by copying files into a sub-directory. Additional pages are brought from disk to memory as required, and a page fault with LRU replacement policy is handled if the memory is full.


# Execution

`make clean`

`make make mysh framesize=X varmemsize=Y` substituting with size of memory for frame storage and variable storage

**Interactive mode**

`./mysh` and follow the help menu that will print

**Batch mode**

`./mysh < testfile.txt` where the testfile contains permitted commands
