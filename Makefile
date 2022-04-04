mysh: shell.c interpreter.c shellmemory.c pcb.c kernel.c cpu.c 
	gcc -D FRAME_MEM_SIZE=$(framesize) -D VAR_MEM_SIZE=$(varmemsize) -c shell.c shellmemory.c interpreter.c pcb.c kernel.c cpu.c virtual_memory.c
	gcc -o mysh shell.o interpreter.o shellmemory.o pcb.o cpu.o kernel.o virtual_memory.o

clean: 
	rm mysh; rm *.o


#alias r="make clean ; make mysh framesize=21 varmemsize=10 ; ./mysh"
#exec prog7 prog8 prog9 RR
