#include "SortedList.h"
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
    if (list == NULL || list->key != NULL || element == NULL) return;//return
    SortedListElement_t *temp = list;
    if (list->prev->next != temp || list->next->prev != temp) return;
    temp = list->next;
    while (temp != NULL && temp->key != NULL){
        if (temp->prev->next != temp || temp->next->prev != temp) return;
        if (strcmp(temp->key, element->key) >0)
            break;
        else temp = temp->next;
    }
    if (opt_yield & INSERT_YIELD){
        sched_yield();
    }
    SortedListElement_t *tprev = temp->prev;
    tprev->next = element;
    element->next = temp;
    temp->prev = element;
    element->prev = tprev;

}


int SortedList_delete( SortedListElement_t *element){
    if (element == NULL || element->key == NULL) return 2;
    if (element->prev->next != element || element->next->prev != element) return 2;
    if (opt_yield & DELETE_YIELD) sched_yield();//TODO what is this?
    element->prev->next = element->next;
    element->next->prev = element->prev;
    return 0;
}


SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){
    //fprintf(stderr,"key: %s",key);//todo delete
    if (list == NULL||list->next == NULL||key == NULL) return NULL;
    SortedListElement_t *temp = list->next;
    while (temp->key != NULL){
        //fprintf(stderr,"temp: %s",temp->key);//todo delete
        if (strcmp(temp->key,key) == 0) 
        return temp;
        if (opt_yield & LOOKUP_YIELD)
            sched_yield();
        temp = temp->next;
    }
    return NULL;
}


int SortedList_length(SortedList_t *list){
    int len = 0;
    if (list->next->key == NULL) return 0;
    SortedListElement_t *temp = list->next;
    while (temp->key != NULL){
        if (temp->prev->next != temp || temp->next->prev != temp) return -1;
        len++;
        if (opt_yield & LOOKUP_YIELD)
            sched_yield();
        temp = temp->next;
    }
    return len;
}


