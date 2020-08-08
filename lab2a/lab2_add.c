#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>

unsigned long diff = 0;
long long counter = 0;
int thre_num = 1;
int iter_num = 1;
int opt_yield = 0;
int opt;
int opt_fault = 0;
int sync_flag = 0;
int spinlock = 0;
char sync_type;
struct timespec begin;
struct timespec fin;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


static struct option options[] = {
    {"threads",       1,  0,  't' },
    {"iterations",    1,  0,  'i' },
    {"yield",         0,  0,  'y' },
    {"sync",          1,  0,  's' },
    {0,               0,  0,   0  }
};

void print_err(int errnum, char* errstr) {
    if (errnum != 0){
        fprintf(stderr,"Error in: %s", errstr);
        if (errnum == 1) exit(1);
        if (errnum == 2) exit(2);
        exit(1);
    }

}

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (opt_yield) sched_yield();
    *pointer = sum;
}

void atomic_add(long long *pointer, long long value) {
    long long before, after;
    do {
        before = *pointer;
        after = before + value;
        if (opt_yield)  sched_yield();
    } while(__sync_val_compare_and_swap(pointer, before, after) != before);
}

void worker(){
    if (sync_flag){
        if (sync_type == 'm'){
            for (int i = 0; i < iter_num; i++){
                pthread_mutex_lock(&mutex);
                add(&counter,1);
                add(&counter,-1);
                pthread_mutex_unlock(&mutex);
            }

        }
        else if (sync_type == 's'){
            for (int i = 0; i < iter_num; i++){
                while (__sync_lock_test_and_set(&spinlock,1));
                add(&counter,1);
                add(&counter,-1);
                __sync_lock_release(&spinlock);
            }
        }
        else if (sync_type == 'c'){
            for (int i = 0; i < iter_num; i++){
                atomic_add(&counter,1);
                atomic_add(&counter,-1);
            }
        }
        else print_err(1, "passing undefined lock type");
    }
    else{
        for (int i = 0; i < iter_num; i++){
            add(&counter,1);
            add(&counter,-1);
        }
    }
}

void print_info() {
    int operations = thre_num*iter_num*2;
    char* fname = "add-none";
    if (sync_flag){
        if (sync_type == 'm'){ fname = opt_yield ? "add-yield-m":"add-m"; }
        if (sync_type == 's'){ fname = opt_yield ? "add-yield-s":"add-s"; }
        if (sync_type == 'c'){ fname = opt_yield ? "add-yield-c":"add-c"; }
    }
    else if (opt_yield) fname = "add-yield-none";
    fprintf(stdout,"%s,%i,%i,%i,%lu,%lu,%lld",fname,thre_num,iter_num,operations,diff,diff/operations,counter);
    fprintf(stdout,"\n");
}

unsigned long calc_time(struct timespec *btime, struct timespec *etime)
{
	unsigned long diff= etime->tv_sec - btime->tv_sec;
	diff = diff * 1000000000 + etime->tv_nsec - btime->tv_nsec;
	return diff;
}


int main(int argc, char ** argv){

    while ((opt = getopt_long(argc, argv, "", options, &opt_fault)) != -1){
        switch(opt){
            case 't':
                thre_num = atoi(optarg);
                break;
                
            case 'i':
                iter_num = atoi(optarg);
                break;

            case 'y':
                opt_yield = 1;
                break;
                
            case 's':
                sync_flag = 1;
                sync_type = (char)*optarg;
                break;
            
            default:
                fprintf(stderr,"Error in finding urrecognized argument\n");
                fprintf(stderr,"Correct Usage: ./lab2_add --threads=number --iterations=number --yield --sync=s/m/c\n");
                exit(1);
                break;
        }
        
    }
    pthread_t mythreads[thre_num];
    
    clock_gettime(CLOCK_MONOTONIC, &begin);

    for (int i = 0; i < thre_num; i++){
        int temp = pthread_create(&mythreads[i],NULL,(void *)&worker,NULL);
        if (temp != 0) print_err(1, "creating thread");
    }

    for (int i = 0; i < thre_num; i++){
        int temp = pthread_join(mythreads[i],NULL);
        if (temp != 0) print_err(1, "joining thread");
    }

    clock_gettime(CLOCK_MONOTONIC, &fin);
    diff = calc_time(&begin,&fin);
    print_info();
    exit(0);
}
