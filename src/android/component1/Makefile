CC = arm-linux-androideabi-gcc
#CC = gcc

atrace:atrace.o trace.o chpath.o
	$(CC) -o atrace atrace.o trace.o chpath.o
atrace.o:atrace.c
	echo $(PATH)
	$(CC) -c atrace.c
trace.o:trace.c
	$(CC) -c trace.c
chpath.o:chpath.c
	$(CC) -c chpath.c
	
clean:
	rm atrace *.o
