.SILENT:

default: 
	gcc -g -Wall -Wextra -lmraa lab4c_tcp.c -o lab4c_tcp -lm
	gcc -g -Wall -Wextra -lmraa -lssl -lcrypto lab4c_tls.c -o lab4c_tls -lm


clean:
	rm -rf lab4c_tcp lab4c_tls lab4c-905129084.tar.gz *.o

dist: 
	tar -czf lab4c-905129084.tar.gz lab4c_tcp.c lab4c_tls.c Makefile README
