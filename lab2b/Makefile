#NAME: Yuxin Wang
#EMAIL: yuxinwanghailey@outlook.com
#ID: 905129084

CC = gcc
CCFLAG = -Wall -Wextra -g -pthread
TARGET = lab2_list.c profile.out lab2b_list.csv  Makefile README SortedList.c SortedList.h lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png lab2b_list.gp 
EXTRA = lab2_list lab2b-905129084.tar.gz *.o


.SILENT:

all:
	$(CC) $(CCFLAG) SortedList.c -o lab2_list lab2_list.c

clean:
	rm -f $(EXTRA)

tests: all
	rm -f lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s   >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=1  --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=1   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=2   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=4   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=8   --yield=id --lists=4   >> lab2b_list.csv
	-./lab2_list --threads=16 --iterations=16  --yield=id --lists=4   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=10   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=20   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=40   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=80   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=10   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=20   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=40   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=80   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=10   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=20   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=40   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=80   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=10   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=20   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=40   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=80   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=10   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=20   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=40   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=80   --yield=id --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=10   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=20   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=40   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=80   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=10   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=20   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=40   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=80   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=10   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=20   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=40   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=80   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=10   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=20   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=40   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=80   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=10   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=20   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=40   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=16 --iterations=80   --yield=id --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=4  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=8  --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=16 --sync=m   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=4  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=8  --sync=s   >> lab2b_list.csv
	./lab2_list --threads=1  --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=2  --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=4  --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=8  --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv
	./lab2_list --threads=12 --iterations=1000  --lists=16 --sync=s   >> lab2b_list.csv

profile:
	rm -f ./raw.gperf profile.out
	LD_PRELOAD=/usr/lib64/libprofiler.so  CPUPROFILE=./raw.gperf  ./lab2_list --iterations=1000 --threads=12 --sync=s >>/dev/null
	pprof --text ./lab2_list ./raw.gperf > profile.out
	pprof --list=worker ./lab2_list ./raw.gperf >> profile.out
	rm -f ./raw.gperf

graphs: tests
	rm -f lab2b_*.png
	chmod +x lab2b_list.gp
	./lab2b_list.gp

dist: graphs profile
	tar -czf lab2b-905129084.tar.gz $(TARGET)