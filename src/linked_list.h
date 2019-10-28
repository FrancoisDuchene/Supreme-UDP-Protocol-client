#ifndef _LINKED_LIST_
#define _LINKED_LIST_

#include <time.h>
#include <stdlib.h>

#include "packet_interface.h"

typedef struct pktList {
    struct node *first;
    struct node *last;
} pktList;

struct node {
    struct node* next;
    pkt_t * currentPkt;
    struct timespec time;
};

pktList* new_pktlist();

/*
 *  Libère la liste ainsi que les paquets contenus en mémoire
 *  @pre: list != NULL
 */
void del_pktlist(pktList *list);

/* 
 *  Rajoute un élément à la queue
 *  @pre: list != NULL 
 *  @pre: pkt != NULL
 */
void enqueue(pktList *list, pkt_t* pkt, struct timespec time);

/*
 *  Retire le premier élément de la queue et place le paquet dans retval
 *  et le time dans rettime
 *  @pre: list != NULL
 *  @pre: retval et rettime ne doivent pas être initialisés
 */
pkt_t* dequeue(pktList *list, struct timespec *rettime);

#endif