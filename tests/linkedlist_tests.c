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
    if(list == NULL) {
        fprintf(stderr, "Plus de mémoire pour les tests\n");
        return;
    }
    list->first = (struct node*) malloc(sizeof(struct node));
    if(list->first == NULL) {
        fprintf(stderr, "Plus de mémoire pour les tests\n");
        del_pktlist(list);
        return;
    }
    list->last = list->first;
    del_pktlist(list);
}

void test_linked_enqueue() {
    pktList *list = new_pktlist();
    if(list == NULL) {
        fprintf(stderr, "Plus de mémoire pour les tests\n");
        return;
    }
    struct node *premier = (struct node*) malloc(sizeof(struct node));
    if(premier == NULL) {
        fprintf(stderr, "Plus de mémoire pour les tests\n");
        del_pktlist(list);
        return;
    }
    list->first = premier;
    CU_ASSERT_PTR_NOT_NULL_FATAL(list->first);
    list->first->currentPkt = (pkt_t*) malloc(sizeof(pkt_t));
    CU_ASSERT_PTR_NOT_NULL_FATAL(list->first->currentPkt);
    list->first->next = NULL;
    list->last = list->first;

    // Deuxième noeud
    pkt_t *pkt = pkt_new();
    if(pkt == NULL) {
        fprintf(stderr, "Plus de mémoire pour les tests\n");
        del_pktlist(list);
        free(premier);
        return;
    }
    time_t t = time(NULL);
    enqueue(list, pkt, t, sizeof(pkt_t));
    CU_ASSERT_EQUAL(list->first, premier);
    CU_ASSERT_NOT_EQUAL(list->last, premier);
    CU_ASSERT_PTR_NOT_NULL(list->first->next);

    // Troisième noeud
    pkt_t *pkt_2 = pkt_new();
    if(pkt_2 == NULL) {
        fprintf(stderr, "Plus de mémoire pour les tests\n");
        del_pktlist(list);
        free(premier);
        pkt_del(pkt);
        return;
    }
    enqueue(list, pkt_2, t, sizeof(pkt_t));
    CU_ASSERT_EQUAL(list->first, premier);
    CU_ASSERT_NOT_EQUAL(list->last, premier);
    CU_ASSERT_PTR_NOT_NULL(list->first->next);
    CU_ASSERT_PTR_NULL(list->last->next);
    CU_ASSERT_PTR_NOT_NULL(list->first->next->next);

    del_pktlist(list);
    free(premier);
    pkt_del(pkt);
    pkt_del(pkt_2);
}

void test_linked_dequeue() {
    pktList *list = new_pktlist();
    if(list == NULL) {
        fprintf(stderr, "Plus de mémoire pour les tests\n");
        return;
    }
    struct node *premier = (struct node*) malloc(sizeof(struct node));
    if (premier == NULL) {
        fprintf(stderr, "Plus de mémoire pour les tests\n");
        del_pktlist(list);
        return;
    }
    list->first = premier;
    CU_ASSERT_PTR_NOT_NULL_FATAL(list->first);
    list->first->currentPkt = (pkt_t*) malloc(sizeof(pkt_t));
    if(list->first->currentPkt == NULL) {
        fprintf(stderr, "Plus de mémoire pour les tests\n");
        del_pktlist(list);
        free(premier);
        return;
    }
    list->first->next = NULL;
    list->last = list->first;

    pkt_t* retval = NULL;
    time_t rettime = 0;
    retval = dequeue(list, &rettime);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_PTR_NULL(list->first);
    CU_ASSERT_PTR_NULL(list->last);
    CU_ASSERT_PTR_NOT_NULL(retval);

    free(premier);
}