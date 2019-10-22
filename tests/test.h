// Auteurs : Duchêne François, Reginster Guillaume
#ifndef TEST_H
#define TEST_H

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <arpa/inet.h>
#include "../src/packet_interface.h"
#include "../src/linked_list.h"

// test format
void test_pkt_new();
void test_pkt_getters();
void test_set_type();
void test_set_tr();
void test_set_window();
void test_set_seqnum();
void test_set_length();
void test_set_timestamp();
void test_set_crc();
void test_pkt_set_payload();
void test_varuint_len();
void test_varuint_predict_len();
void test_predict_header_length();
void test_varuint_decode();
void test_varuint_encode();
void test_encode_decode();
// test linked list
void test_linked_list_new();
void test_linked_list_del();
void test_linked_enqueue();
void test_linked_dequeue();
#endif
