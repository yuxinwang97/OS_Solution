CC = gcc
CCFLAG = -Wall -Wextra -g
TARGET = lab1a.c Makefile README

.SILENT:

.PHONY: all
all:
	$(CC) $(CCFLAG) -o lab1a lab1a.c

.PHONY: dist
dist: 
	tar -czf lab1a-905129084.tar.gz $(TARGET)

.PHONY: clean
clean:
	rm -f lab1a lab1a-905129084.tar.gz *.o