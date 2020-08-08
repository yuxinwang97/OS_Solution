#include <stdio.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include "SortedList.h"
int opt_yield = 0;
 /* 
#define	INSERT_YIELD	0x01	// yield in insert critical section
#define	DELETE_YIELD	0x02	// yield in delete critical section
#define	LOOKUP_YIELD	0x04	// yield in lookup/length critical esction


struct SortedListElement {
	struct SortedListElement *prev;
	struct SortedListElement *next;
	const char *key;
};
typedef struct SortedListElement SortedList_t;
typedef struct SortedListElement SortedListElement_t;



void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
    if (list == NULL || list->key != NULL || element == NULL) exit(1);
    SortedListElement_t temp = list;
    if (list->prev->next != temp || list->next->prev != temp) exit(1);
    temp = list->next;
    while (temp != NULL && temp->key != NULL){
        if (temp->prev->next != temp || temp->next->prev != temp) exit(1);
        if (strcmp(temp->key, element->key) >0)
            break;
        else temp = temp->next;
    }
    if (opt_yield & INSERT_YIELD){
        sched_yield();
    }
    SortedListElement_t tprev = temp->prev;
    tprev->next = element;
    element->next = temp;
    temp->prev = element;
    element->prev = tprev;

}


int SortedList_delete( SortedListElement_t *element){
    if (element == NULL || element->key == NULL) return 1;
    if (element->prev->next != element || element->next->prev != element) return 1;
    if (opt_yield & DELETE_YIELD) sched_yield();//TODO what is this?
    element->prev->next = element->next;
    element->next->prev = element->prev;
    return 0;
}


SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){
    if (list == NULL||list->next == NULL) return 1;
    //if (list->next->key == key) 
    SortedListElement_t temp = list;
    while (temp->key != NULL){
        if (strcmp(temp->key,key) == 0) return temp;
        if (opt_yield & LOOKUP_YIELD)
            sched_yield();
        temp = temp->next;
    }
    return NULL;
}


int SortedList_length(SortedList_t *list){
    int len = 0;
    if (list->next->key == NULL) return 0;
    SortedListElement_t temp = list->next;
    while (temp->key != NULL){
        if (temp->prev->next != temp || temp->next->prev != temp) return -1;
        len++;
        if (opt_yield & LOOKUP_YIELD)
            sched_yield();
        temp = temp->next;
    }
    return NULL;
}

*/


int main(int argc, char ** argv){
    SortedList_t *head = (SortedList_t *) malloc(sizeof(SortedList_t));
    head->next = head;
    head->prev = head;
    head->key = NULL;

    char *value1 = "1";
    char *value2 = "2";
    char *value3 = "3";

    SortedListElement_t *ele1 = (SortedListElement_t *) malloc(sizeof(SortedListElement_t));
    ele1->key = value1;

    SortedListElement_t *ele2 = (SortedListElement_t *) malloc(sizeof(SortedListElement_t));
    ele2->key = value2;

    SortedListElement_t *ele3 = (SortedListElement_t *) malloc(sizeof(SortedListElement_t));
    ele3->key = value3;

    int length =  SortedList_length(head);
    fprintf(stderr,"%i\n",length);

    SortedList_insert(head, ele1);
    SortedList_insert(head, ele2);
    SortedList_insert(head, ele3);

    length =  SortedList_length(head);
    fprintf(stderr,"%i\n",length);


    fprintf(stderr,"ele1: %s\n",ele1->key);
    fprintf(stderr,"ele2: %s\n",ele2->key);
    fprintf(stderr,"ele3: %s\n",ele3->key);
    if (head->next == ele1) fprintf(stderr,"true1\n");
    if (ele1->next == ele2) fprintf(stderr,"true2\n");
    if (ele2->next == ele3) fprintf(stderr,"true3\n");
    if (ele3->next == head) fprintf(stderr,"true3\n");
    if (ele1->prev == head) fprintf(stderr,"true1\n");
    if (ele2->prev == ele1) fprintf(stderr,"true2\n");
    if (ele3->prev == ele2) fprintf(stderr,"true3\n");
    if (head->prev == ele3) fprintf(stderr,"true4\n");
    fprintf(stderr,"hello world!\n");
    if (head->next == ele3) fprintf(stderr,"true1\n");
    if (ele3->next == ele2) fprintf(stderr,"true2\n");
    if (ele2->next == ele1) fprintf(stderr,"true3\n");
    if (ele1->next == head) fprintf(stderr,"true4\n");
    if (ele3->prev == head) fprintf(stderr,"true1\n");
    if (ele2->prev == ele3) fprintf(stderr,"true2\n");
    if (ele1->prev == ele2) fprintf(stderr,"true3\n");
    if (head->prev == ele1) fprintf(stderr,"true4\n");



    char *la = "2";
    SortedListElement_t *findit = SortedList_lookup(head, la);

    if (findit == NULL) {fprintf(stderr,"NULL\n");}
    else {fprintf(stderr,"findit: %s\n",findit->key);}

    SortedList_delete( ele2);

    findit = SortedList_lookup(head, la);

    if (findit == NULL) {fprintf(stderr,"NULL\n");}
    else {fprintf(stderr,"findit: %s\n",findit->key);}

    return 0;
}


