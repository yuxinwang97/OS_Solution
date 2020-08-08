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
unsigned long diff = 0;
long long counter = 0;
int thre_num = 1;
int iter_num = 1;
int opt_yield = 0;
int opt;
int opt_fault = 0;
int sync_flag = 0;
int yname_flag = 0;
int spinlock = 0;
char sync_type;
char* yname;
char** keyarray;
char* keypool = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0";
//TODO handle sigfault, using signal

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
SortedList_t* head;
SortedListElement_t* body;


static struct option options[] = {
    {"threads",       1,  0,  't' },
    {"iterations",    1,  0,  'i' },
    {"yield",         1,  0,  'y' },
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

//TODO correct return value for each case;
void handler(int signum){
    if (signum == SIGSEGV){
        fprintf(stderr, "Error: Find inconsistencies in the list\n");
        exit(2);
    }
}

char* randkey(){
    char* tempkey = (char*) malloc(2*sizeof(char));//free char
    int tempnum= rand()%62;
    tempkey[0] = keypool[tempnum];
    tempkey[1] = '\0';
    return tempkey;
}

void init_list(){
    head = (SortedList_t*) malloc(sizeof(SortedList_t));
    head->next = head;
    head->prev = head;
    head->key = NULL;
    int sum = thre_num*iter_num;
    body = (SortedListElement_t*) malloc(sum*sizeof(SortedListElement_t));
    for (int i = 0; i < sum; i++){
        body[i].key = randkey();
    }
}

void worker(void* tpid){
    int base = *((int *)tpid);
    base *= iter_num;
    for (int i = 0; i < thre_num; i++){
        if (sync_flag){
            if (sync_type == 'm'){
                pthread_mutex_lock(&mutex);
                SortedList_insert(head, &body[base+i]);
                pthread_mutex_unlock(&mutex);
            }
            else if (sync_type == 's'){
                while(__sync_lock_test_and_set(&spinlock, 1));
                SortedList_insert(head, &body[base+i]);
                __sync_lock_release(&spinlock);
            }
            else print_err(1, "unrecognized option for argument sync\n");
        }
        else  SortedList_insert(head, &body[base+i]);
    }

    int ls_len = 0;
    if (sync_flag){
        if (sync_type == 'm'){
            pthread_mutex_lock(&mutex);
            ls_len = SortedList_length(head);
            pthread_mutex_unlock(&mutex);
        }
        else if (sync_type == 's'){
            while(__sync_lock_test_and_set(&spinlock, 1));
            ls_len = SortedList_length(head);
            __sync_lock_release(&spinlock);
        }
        else print_err(1, "unrecognized option for argument sync\n");
    }
    else  {ls_len = SortedList_length(head);}
    if (ls_len < 0 ) print_err(2,"finding negative length\n");

    for (int i = 0; i < thre_num; i++){
        if (sync_flag){
            if (sync_type == 'm'){
                pthread_mutex_lock(&mutex);
                SortedListElement_t* temp_element = SortedList_lookup(head, body[base+i].key);
                if (temp_element == NULL) {print_err(2,"cant not look up element\n");}
                else {
                    int ret_val = SortedList_delete(temp_element);
                    if (ret_val == 2) print_err(2,"cant delete element\n");//the return value is??????
                }
                pthread_mutex_unlock(&mutex);
            }
            else if (sync_type == 's'){
                while(__sync_lock_test_and_set(&spinlock, 1));
                SortedListElement_t* temp_element = SortedList_lookup(head, body[base+i].key);
                if (temp_element == NULL) {print_err(2,"cant not look up element\n");}
                else {
                    int ret_val = SortedList_delete(temp_element);
                    if (ret_val == 2) print_err(2,"cant delete element\n");//the return value is??????
                }
                __sync_lock_release(&spinlock);
            }
            else print_err(1, "unrecognized option for argument sync\n");
        }
        else {
            SortedListElement_t* temp_element = SortedList_lookup(head, body[base+i].key);
                if (temp_element == NULL) {print_err(2,"cant not look up element\n");}
                else {
                    int ret_val = SortedList_delete(temp_element);
                    if (ret_val == 2) print_err(2,"cant delete element\n");//the return value is??????
                }
        }
    }
}

void print_info() {
    int operations = thre_num*iter_num*3;
    char* fname = "list-";
    yname = (opt_yield == 0) ? "none" : yname;//TODO err checking
    char* sname;
    if (sync_flag){
       //fprintf(stderr,"sync_type: %c\n",sync_type);
        if (sync_type == 'm'){ sname = "-m"; }
        else if (sync_type == 's'){ sname = "-s"; }
        else print_err(1,"finding unrecognized option in sync\n");
    }
    else sname = "-none";
    fprintf(stdout,"%s%s%s,%i,%i,1,%i,%lu,%lu\n",fname,yname,sname,thre_num,iter_num,operations,diff,diff/operations);
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

    if(SortedList_length(head) != 0){
        print_err(2,"the length is not 0 after finish\n");
    }


    diff = calc_time(&begin,&fin);
    print_info();


    if (yname_flag) free(yname);
    free(head);
    int freesum = thre_num*iter_num;
    for (int i = 0; i < freesum; i++){
        free((void *)body[i].key);
    }
    
    free(body);
    exit(0);
}
