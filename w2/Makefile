CC = gcc
CFLAGS = -Wall -O2
OBJS = sysinfo.o main.o
TARGET = sysinfo

all : main

sysinfo.o : sysinfo.h sysinfo.c
	$(CC) $(CFLAGS) -c sysinfo.c

main.o : sysinfo.h main.c
	$(CC) $(CFLAGS) -c main.c

main : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) *.o core

clear:
	rm -f $(OBJS) $(TARGET) *.o core