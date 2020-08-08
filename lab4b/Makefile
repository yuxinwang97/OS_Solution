CC = gcc
CCFLAG =  -g -Wall -Wextra -lmraa -lm 
TARGET = lab4b.c Makefile README
EXTRA = lab4b lab4b-905129084.tar.gz *.o

.SILENT:

all:
	$(CC) $(CCFLAG) -o lab4b lab4b.c

prepare:
	touch test.txt

check: all prepare
	{ sleep 3; echo "SCALE=F"; echo "PERIOD=2"; echo "LOG TEST"; sleep 1; echo "STOP"; sleep 1; echo "START"; sleep 2; echo "OFF"; } | ./lab4b --scale=C --log=test.txt
	RESULT=$$? ; \
	if [ $$RESULT==0 ]; then \
	echo "Vaild return code "; \
	else echo "Failed to execute commands "; \
	fi

	if [ $(shell grep "SCALE=F" test.txt)="SCALE=F" ]; then \
	echo "command succeed: SCALE=F "; \
	else echo "Failed execute command: SCALE=F "; \
	fi

	if [ $(shell grep "PERIOD=2" test.txt)="PERIOD=2" ]; then \
	echo "command succeed: PERIOD=2"; \
	else echo "Failed execute command: PERIOD=2 "; \
	fi

	if [ $(shell grep "LOG TEST" test.txt)="LOG TEST" ]; then \
	echo "command succeed: LOG TEST"; \
	else echo "Failed execute command: LOG TEST "; \
	fi

	if [ $(shell grep "STOP" test.txt)="STOP" ]; then \
	echo "command succeed: STOP"; \
	else echo "Failed execute command: STOP "; \
	fi

	if [ $(shell grep "START" test.txt)="START" ]; then \
	echo "command succeed: START"; \
	else echo "Failed execute command: START "; \
	fi
	
	if [ $(shell grep "OFF" test.txt)="OFF" ]; then \
	echo "command succeed: OFF"; \
	else echo "Failed execute command: OFF "; \
	fi

	rm -f test.txt

clean:
	rm -f $(EXTRA)

dist: 
	tar -czf lab4b-905129084.tar.gz $(TARGET)