EXEC=nqueen
CC=gcc
DBG_CFLAGS= #-D_DEBUG -g
CFLAGS= -pthread $(DBG_CFLAGS) 

all:
	$(CC) $(CFLAGS) main.c -o $(EXEC)


clean:
	rm -f $(EXEC)
