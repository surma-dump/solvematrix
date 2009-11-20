all:
	gcc *.c -o matrix  -ggdb
run:
	gdb ./matrix 
clean:
	-@rm *.o matrix
