#include "test.h"

void test_linked_list_new() {
    pktList *list = new_pktlist();
    CU_ASSERT_PTR_NOT_NULL_FATAL(list);
    CU_ASSERT_PTR_NULL(list->first);
    CU_ASSERT_PTR_NULL(list->last);
    free(list);
}

void test_linked_list_del() {
    pktList *list = new_pktlist();
    list->first = (struct node*) malloc(sizeof(struct node));
    CU_ASSERT_PTR_NOT_NULL_FATAL(list->first);
    list->last = list->first;
    del_pktlist(list);
    CU_ASSERT_PTR_NULL(list);
}

void test_linked_enqueue() {
    pktList *list = new_pktlist();
    struct node *premier = (struct node*) malloc(sizeof(struct node));
    list->first = premier;
    CU_ASSERT_PTR_NOT_NULL_FATAL(list->first);
    list->first->currentPkt = (pkt_t*) malloc(sizeof(pkt_t));
    CU_ASSERT_PTR_NOT_NULL_FATAL(list->first->currentPkt);
    list->first->next = NULL;
    list->last = list->first;

    // Deuxième noeud
    pkt_t *pkt = pkt_new();
    struct timespec time;
    enqueue(list, pkt, time);
    CU_ASSERT_EQUAL(list->first, premier);
    CU_ASSERT_NOT_EQUAL(list->last, premier);
    CU_ASSERT_PTR_NOT_NULL(list->first->next);

    // Troisième noeud
    pkt_t *pkt_2 = pkt_new();
    enqueue(list, pkt_2, time);
    CU_ASSERT_EQUAL(list->first, premier);
    CU_ASSERT_NOT_EQUAL(list->last, premier);
    CU_ASSERT_PTR_NOT_NULL(list->first->next);
    CU_ASSERT_PTR_NULL(list->last->next);
    CU_ASSERT_PTR_NOT_NULL(list->first->next->next);
}

void test_linked_dequeue() {
    pktList *list = new_pktlist();
    struct node *premier = (struct node*) malloc(sizeof(struct node));
    list->first = premier;
    CU_ASSERT_PTR_NOT_NULL_FATAL(list->first);
    list->first->currentPkt = (pkt_t*) malloc(sizeof(pkt_t));
    CU_ASSERT_PTR_NOT_NULL_FATAL(list->first->currentPkt);
    list->first->next = NULL;
    list->last = list->first;

    pkt_t* retval = NULL;
    struct timespec *rettime = 0;
    dequeue(list, retval, rettime);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_PTR_NULL(list->first);
    CU_ASSERT_PTR_NULL(list->last);
}