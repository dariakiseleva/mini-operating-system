mysh: shell.c interpreter.c shellmemory.c pcb.c kernel.c cpu.c 
	gcc -D FRAME_MEM_SIZE=$(framesize) -D VAR_MEM_SIZE=$(varmemsize) -c shell.c shellmemory.c
	gcc -c interpreter.c pcb.c kernel.c cpu.c virtual_memory.c
	gcc -o mysh shell.o interpreter.o shellmemory.o pcb.o cpu.o kernel.o virtual_memory.o

clean: 
	rm mysh; rm *.o


#make mysh framesize=210 varmemsize=100
#exec prog1 prog2 prog3 RR
