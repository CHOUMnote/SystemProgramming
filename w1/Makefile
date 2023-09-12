CC = gcc
CFLAGS = -Wall -O2
OBJS = dir.o main.o
TARGET = myls

all : main

dir.o : dir.h dir.c
	$(CC) $(CFLAGS) -c dir.c

main.o : dir.h main.c
	$(CC) $(CFLAGS) -c main.c

main : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) *.o core

clear:
	rm -f $(OBJS) $(TARGET) *.o core