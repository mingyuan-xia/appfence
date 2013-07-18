CC = arm-linux-androideabi-gcc

atrace:atrace.o
	$(CC) -o atrace atrace.o
atrace.o:atrace.c
	$(CC) -c atrace.c
	
clean:
	rm atrace *.o
