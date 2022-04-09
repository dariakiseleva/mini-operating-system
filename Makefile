mysh: shell.c interpreter.c shellmemory.c pcb.c kernel.c cpu.c 
	gcc -D FRAME_MEM_SIZE=$(framesize) -D VAR_MEM_SIZE=$(varmemsize) -c shell.c shellmemory.c interpreter.c pcb.c kernel.c cpu.c 
	gcc -o mysh shell.o interpreter.o shellmemory.o pcb.o cpu.o kernel.o

clean: 
	rm mysh; rm *.o



# TEST 1
# make clean; make mysh framesize=18 varmemsize=10; ./mysh
# exec A3_testcases_public/prog1 A3_testcases_public/prog2 A3_testcases_public/prog3 RR

#

# TEST 2
# 

# TEST 3
# 

# TEST 4
# 

# TEST 5
# 

