#include "linked_list.h"

pktList* new_pktlist() {
    struct pktList *result = (struct pktList *) malloc(sizeof(pktList));
    if(result == NULL) {
        fprintf(stderr, "Not enough memory\n");
        return NULL;
    }
    
    //noeud->time = NULL;

    result->first = NULL;
    result->last = NULL;

    return result;
}

void del_pktlist(pktList *list) {
    if(list == NULL) {
        return;
    }
    struct node* current = list->first;
    struct node* tmp = NULL;
    while(current != NULL) {
        pkt_del(current->currentPkt);
        tmp = current->next;
        free(current);
        current = tmp;
    }
    free(list);
    list = NULL;
}

void enqueue(pktList *list, pkt_t* pkt, time_t time, size_t size) {
    if(list == NULL || pkt == NULL) {
        return;
    }
    struct node *noeud = (struct node *) malloc(sizeof(struct node));
    if(noeud == NULL) return;

    noeud->currentPkt = pkt;
    noeud->time = time;
    noeud->size = size;
    noeud->next = NULL;

    if(list->first == NULL) {
        list->first = noeud;
        list->last = noeud;
    }else{
        list->last->next = noeud;
        list->last = noeud;
    }    
}

pkt_t* dequeue(pktList *list, time_t *rettime) {
    if(list == NULL) {
        rettime = NULL;
        return NULL;
    }
    struct node *noeud = list->first;

    pkt_t* temp = noeud->currentPkt;
    
    rettime = &(noeud->time);

    if(list->last == list->first) {
        list->first = NULL;
        list->last = NULL;
        
    }else{
        list->first = list->first->next;
    }
    
    free(noeud);
    return temp;
}