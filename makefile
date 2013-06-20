#
# Makefile for emhttp
#
CC = gcc
OBJS =	emsnmp.o snmpdata.o user.o
all:	emsnmp
emsnmp: $(OBJS)
	$(CC) -o emsnmp $(OBJS)
.c.o:
	$(CC) $(CFLAGS) -Wall -c $<
clean:
	rm -f emsnmp *.o
