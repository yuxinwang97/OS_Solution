#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include <signal.h> 
#include "SortedList.h"

#define	INSERT_YIELD	0x01	// yield in insert critical section
#define	DELETE_YIELD	0x02	// yield in delete critical section
#define	LOOKUP_YIELD	0x04	// yield in lookup/length critical esction

struct timespec begin;
struct timespec fin;
struct timespec *lock_s;
struct timespec *lock_f;
unsigned long long diff = 0;
long long counter = 0;
unsigned long long *locktime;
int thre_num = 1;
int iter_num = 1;
int list_num = 1;
int opt_yield = 0;
int opt;
int sum;
int opt_fault = 0;
int sync_flag = 0;
int yname_flag = 0;
int* spinlock;
int* hash;
char sync_type;
char* yname;
char** keyarray;
char* keypool = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0";


pthread_mutex_t* mutex;
SortedList_t* head;
SortedListElement_t* body;


static struct option options[] = {
    {"threads",       1,  0,  't' },
    {"iterations",    1,  0,  'i' },
    {"yield",         1,  0,  'y' },
    {"sync",          1,  0,  's' },
    {"lists",          1,  0,  'l'  },
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

void handler(int signum){
    if (signum == SIGSEGV){
        fprintf(stderr, "Error: Sigment fault, may caused by finding inconsistencies in the list\n");
        exit(2);
    }
}

char* randkey(){
    char* tempkey = (char*) malloc(2*sizeof(char));
    int tempnum= rand()%62;
    tempkey[0] = keypool[tempnum];
    tempkey[1] = '\0';
    return tempkey;
}

int hashNum(const char* key){
    return key[0] % list_num;
}

void init_list(){
    head = malloc(sizeof(SortedList_t)*list_num);
    sum = thre_num*iter_num;
    body = malloc(sum*sizeof(SortedListElement_t));
    hash = malloc(sum*sizeof(int));
    lock_s = malloc(sizeof(struct timespec)*list_num);
    lock_f = malloc(sizeof(struct timespec)*list_num);
    locktime = malloc(sizeof(long long)*list_num);
    for (int i = 0; i < list_num; i++){
        head[i].next = &head[i];
        head[i].prev = &head[i];
        head[i].key = NULL;
        locktime[i] = 0;
        clock_gettime(CLOCK_MONOTONIC, &lock_s[i]);
        clock_gettime(CLOCK_MONOTONIC, &lock_f[i]);
    }
    for (int i = 0; i < sum; i++){
        body[i].key = randkey();
    }
    for (int i = 0; i < sum; i++){
        hash[i] = hashNum(body[i].key);
    }
}

void set_lock(){
    if (sync_flag){
            if (sync_type == 'm'){
                mutex = malloc(sizeof(pthread_mutex_t)*list_num);
                if (mutex == NULL) print_err(1,"malloc memory for mutex lock");
                for (int i = 0; i < list_num; i++) {
	      		    if (pthread_mutex_init(&mutex[i], NULL) != 0) 
                        print_err(1,"initializing mutex lock");
	    	    }
            }
            else if (sync_type == 's'){
                spinlock = malloc(sizeof(int)*list_num);
                if (spinlock == NULL) print_err(1,"malloc memory for spinlock");
                for (int i = 0; i < list_num; i++) {
	      		    spinlock[i] = 0;
	    	    }
            }
            else print_err(1, "unrecognized option for argument sync\n");
        }
}

unsigned long long calc_time(struct timespec *btime, struct timespec *etime)
{
    long long temp_sec = etime->tv_sec - btime->tv_sec;
    long long temp_nano = etime->tv_nsec - btime->tv_nsec;
	diff = temp_sec * 1000000000 +temp_nano;
    return diff;
}


void worker(void* tpid){
    int base = *((int *)tpid);
    for (int i = base; i < sum; i += thre_num){
        int temp = hash[i];
        if (sync_flag){
            if (sync_type == 'm'){
                if (clock_gettime(CLOCK_MONOTONIC, &lock_s[temp]) < 0) print_err(1,"setting up start timer for mutex lock");
                pthread_mutex_lock(&mutex[temp]);
                if (clock_gettime(CLOCK_MONOTONIC, &lock_f[temp]) < 0) print_err(1,"setting up end timer for mutex lock");
                SortedList_insert(&head[temp], &body[i]);
                long long temp_sec = lock_f[temp].tv_sec - lock_s[temp].tv_sec;
                temp_sec = temp_sec > 0 ? temp_sec : 0;
                long long temp_nano = lock_f[temp].tv_nsec - lock_s[temp].tv_nsec;
                temp_nano = temp_nano > 0 ? temp_nano : 20;
                locktime[temp] += temp_sec*1000000000 + temp_nano;
                pthread_mutex_unlock(&mutex[temp]);
            }
            else if (sync_type == 's'){
                if (clock_gettime(CLOCK_MONOTONIC, &lock_s[temp]) < 0) print_err(1,"setting up start timer for spinlock");
                while(__sync_lock_test_and_set(&spinlock[temp], 1));
                if (clock_gettime(CLOCK_MONOTONIC, &lock_f[temp]) < 0) print_err(1,"setting up end timer for spinlock");
                SortedList_insert(&head[temp], &body[i]);
                long long temp_sec = lock_f[temp].tv_sec - lock_s[temp].tv_sec;
                temp_sec = temp_sec > 0 ? temp_sec : 0;
                long long temp_nano = lock_f[temp].tv_nsec - lock_s[temp].tv_nsec;
                temp_nano = temp_nano > 0 ? temp_nano : 20;
                locktime[temp] += temp_sec*1000000000 + temp_nano;
                __sync_lock_release(&spinlock[temp]);
            }
            else print_err(1, "unrecognized option for argument sync\n");
        }
        else  SortedList_insert(&head[temp], &body[i]);
    }

    int ls_len = 0;
    if (sync_flag){
        if (sync_type == 'm'){
            for (int i = 0; i < list_num; i++){
                if (clock_gettime(CLOCK_MONOTONIC, &lock_s[i]) < 0) print_err(1,"setting up start timer for mutex lock");
                pthread_mutex_lock(&mutex[i]);
                if (clock_gettime(CLOCK_MONOTONIC, &lock_f[i]) < 0) print_err(1,"setting up end timer for mutex lock"); 
                int sub_len = SortedList_length(&head[i]);
                if (sub_len < 0) print_err(2,"negative length\n");
                else ls_len += sub_len;
                long long temp_sec = lock_f[i].tv_sec - lock_s[i].tv_sec;
                temp_sec = temp_sec > 0 ? temp_sec : 0;
                long long temp_nano = lock_f[i].tv_nsec - lock_s[i].tv_nsec;
                temp_nano = temp_nano > 0 ? temp_nano : 20;
                locktime[i] += temp_sec*1000000000 + temp_nano;
                pthread_mutex_unlock(&mutex[i]);
            }
        }

        else if (sync_type == 's'){
            for (int i = 0; i < list_num; i++){
                if (clock_gettime(CLOCK_MONOTONIC, &lock_s[i]) < 0) print_err(1,"setting up start timer for mutex lock");
                while(__sync_lock_test_and_set(&spinlock[i], 1));
                if (clock_gettime(CLOCK_MONOTONIC, &lock_f[i]) < 0) print_err(1,"setting up end timer for mutex lock"); 
                int sub_len = SortedList_length(&head[i]);
                if (sub_len < 0) print_err(2,"negative length\n");
                else ls_len += sub_len;
                long long temp_sec = lock_f[i].tv_sec - lock_s[i].tv_sec;
                temp_sec = temp_sec > 0 ? temp_sec : 0;
                long long temp_nano = lock_f[i].tv_nsec - lock_s[i].tv_nsec;
                temp_nano = temp_nano > 0 ? temp_nano : 20;
                locktime[i] += temp_sec*1000000000 + temp_nano;
                __sync_lock_release(&spinlock[i]);
            }
            
        }
        else print_err(1, "unrecognized option for argument sync\n");
    }
    else  {
        for (int i = 0; i < list_num; i++){
            int sub_len = SortedList_length(&head[i]);
            if (sub_len < 0) print_err(2,"negative length\n");
            else ls_len += sub_len;
        }
    }
    if (ls_len < 0 ) print_err(2,"finding negative length\n");


    for (int i = base; i < sum; i += thre_num){
        int temp = hash[i];
        if (sync_flag){
            if (sync_type == 'm'){
                if (clock_gettime(CLOCK_MONOTONIC, &lock_s[temp]) < 0) print_err(1,"setting up start timer for mutex lock");
                pthread_mutex_lock(&mutex[temp]);
                if (clock_gettime(CLOCK_MONOTONIC, &lock_f[temp]) < 0) print_err(1,"setting up end timer for mutex lock"); 
                SortedListElement_t* temp_element = SortedList_lookup(&head[temp], body[i].key);
                if (temp_element == NULL) {print_err(2,"can not look up element\n");}
                else {
                    int ret_val = SortedList_delete(temp_element);
                    if (ret_val != 0) print_err(2,"cant delete element\n");
                }
                long long temp_sec = lock_f[temp].tv_sec - lock_s[temp].tv_sec;
                temp_sec = temp_sec > 0 ? temp_sec : 0;
                long long temp_nano = lock_f[temp].tv_nsec - lock_s[temp].tv_nsec;
                temp_nano = temp_nano > 0 ? temp_nano : 20;
                locktime[temp] += temp_sec*1000000000 + temp_nano;
                pthread_mutex_unlock(&mutex[temp]);
            }
            else if (sync_type == 's'){
                if (clock_gettime(CLOCK_MONOTONIC, &lock_s[temp]) < 0) print_err(1,"setting up start timer for spinlock");
                while(__sync_lock_test_and_set(&spinlock[temp], 1));
                if (clock_gettime(CLOCK_MONOTONIC, &lock_f[temp]) < 0) print_err(1,"setting up end timer for spinlock");
                SortedListElement_t* temp_element = SortedList_lookup(&head[temp], body[i].key);
                if (temp_element == NULL) {print_err(2,"can not look up element\n");}
                else {
                    int ret_val = SortedList_delete(temp_element);
                    if (ret_val != 0) print_err(2,"cant delete element\n");
                }
                long long temp_sec = lock_f[temp].tv_sec - lock_s[temp].tv_sec;
                temp_sec = temp_sec > 0 ? temp_sec : 0;
                long long temp_nano = lock_f[temp].tv_nsec - lock_s[temp].tv_nsec;
                temp_nano = temp_nano > 0 ? temp_nano : 20;
                locktime[temp] += temp_sec*1000000000 + temp_nano;
                __sync_lock_release(&spinlock[temp]);
            }
            else print_err(1, "unrecognized option for argument sync\n");
        }
        else {
            SortedListElement_t* temp_element = SortedList_lookup(&head[temp], body[i].key);
            if (temp_element == NULL) {print_err(2,"can not look up element\n");}
            else {
                int ret_val = SortedList_delete(temp_element);
                if (ret_val != 0) print_err(2,"cant delete element\n");
            }
        }
    }
}

void print_info() {
    long long operations = thre_num*iter_num*3;
    long long lock_op = thre_num*(iter_num*2+1);
    unsigned long long locksum = 0;
    for (int i = 0 ; i < list_num; i++){
        //fprintf(stderr,"locksum: %llu,lock%i: %llu", locksum, lock);
        locksum += locktime[i];
    }
    char* fname = "list-";
    yname = (opt_yield == 0) ? "none" : yname;
    char* sname;
    if (sync_flag){
        if (sync_type == 'm'){ sname = "-m"; }
        else if (sync_type == 's'){ sname = "-s"; }
        else print_err(1,"finding unrecognized option in sync\n");
    }
    else sname = "-none";
    fprintf(stdout,"%s%s%s,%i,%i,%i,%lld,%llu,%llu,%llu\n",fname,yname,sname,thre_num,iter_num,list_num,operations,diff,diff/operations,locksum/lock_op);
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

            case 'l':
                list_num = atoi(optarg);
                break;

            case 'y':
                yname = (char*)malloc((8*sizeof(char)));
                yname_flag = 1;
                memcpy(yname,optarg,(8*sizeof(char)));
                for (int i = 0; i < (int)strlen(optarg); i++){
                    switch(optarg[i]){
                        case 'i': 
                            opt_yield = (opt_yield | INSERT_YIELD); 
                            break;
                        case 'd': 
                            opt_yield = (opt_yield | DELETE_YIELD);
                            break;
                        case 'l': 
                            opt_yield = (opt_yield | LOOKUP_YIELD);
                            break;
                        default :
                            print_err(1, "unrecognized optin in yield\n"); 
                            break;  
                    }
                }
                break;
                
            case 's':
                sync_flag = 1;
                sync_type = (char)*optarg;
                break;
            
            default:
                fprintf(stderr,"Error in finding urrecognized argument\n");
                fprintf(stderr,"Correct Usage: ./lab2_list --threads=number --iterations=number --yield=i/d/l --sync=s/m\n");
                exit(1);
                break;
        }
    }

    signal(SIGSEGV, handler);
    pthread_t mythreads[thre_num];
    int mypid[thre_num];
    srand((unsigned int) time(0));
    init_list();
    set_lock();


    clock_gettime(CLOCK_MONOTONIC, &begin);//TODO systemcall error

    for (int i = 0; i < thre_num; i++){
        mypid[i] = i;
        int temp = pthread_create(&mythreads[i],NULL,(void *)&worker,(void *)&mypid[i]);
        if (temp != 0) print_err(1, "creating thread");
    }

    for (int i = 0; i < thre_num; i++){
        int temp = pthread_join(mythreads[i],NULL);
        if (temp != 0) print_err(1, "joining thread");
    }

    clock_gettime(CLOCK_MONOTONIC, &fin);

    for (int i = 0; i < list_num; i++){
        if(SortedList_length(&head[i]) != 0) {print_err(2,"the length is not 0 after finish\n");}
    }


    diff = calc_time(&begin,&fin);
    print_info();
    if (yname_flag) free(yname); 
    if (head != NULL) free(head); 
    if (hash != NULL)  free(hash);
    if (lock_s != NULL) free(lock_s);
    if (lock_f != NULL) free(lock_f);
    if (locktime != NULL) free(locktime);
    for (int i = 0; i < sum; i++){
        if (body[i].key != NULL)
            free((void *)body[i].key);
    }
    if (body != NULL) free(body);
    if (mutex != NULL) free(mutex);
    if (spinlock != NULL) free(spinlock);
    exit(0);
}
