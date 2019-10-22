// Auteurs : Duchêne François, Reginster Guillaume
/*
 *  Fichier de tests unitaires pour les fonctions concernant le format des paquets
 */
#include "test.h"

void test_pkt_new() {
  pkt_t *pkt = pkt_new();
  CU_ASSERT_PTR_NOT_NULL(pkt);
  CU_ASSERT_EQUAL(pkt->window, 0);
	CU_ASSERT_FALSE(pkt->tr);
	CU_ASSERT_FALSE(pkt->type);
	CU_ASSERT_FALSE(pkt->seqnum);
	CU_ASSERT_FALSE(pkt->length);
	CU_ASSERT_FALSE(pkt->timestamp);
	CU_ASSERT_FALSE(pkt->crc1);
	CU_ASSERT_PTR_NULL(pkt->payload);
	CU_ASSERT_FALSE(pkt->crc2);

	pkt_del(pkt);
}

void test_pkt_getters() {
	pkt_t *pkt = pkt_new();
	pkt->window = 1;
	pkt->tr = 0;
	pkt->type = 3;
	pkt->seqnum = 42;
	pkt->length = 5;
	pkt->timestamp = 696;
	pkt->crc1 = 789;
	pkt->payload = (char *) malloc(sizeof("Hello"));
	memcpy(pkt->payload, "Hello", sizeof("Hello"));
	pkt->crc2 = 987;

	CU_ASSERT_EQUAL_FATAL(pkt_get_window(pkt), 1);
	CU_ASSERT_EQUAL_FATAL(pkt_get_tr(pkt), 0);
	CU_ASSERT_EQUAL_FATAL(pkt_get_type(pkt), 3);
	CU_ASSERT_EQUAL_FATAL(pkt_get_seqnum(pkt), 42);
	CU_ASSERT_EQUAL_FATAL(pkt_get_length(pkt), 5);
	CU_ASSERT_EQUAL_FATAL(pkt_get_timestamp(pkt), 696);
	CU_ASSERT_EQUAL_FATAL(pkt_get_crc1(pkt), 789);
	CU_ASSERT_NSTRING_EQUAL_FATAL(pkt_get_payload(pkt), "Hello", 5);
	CU_ASSERT_EQUAL_FATAL(pkt_get_crc2(pkt), 987);

	pkt_del(pkt);
}

void test_set_type() {
	pkt_t *pkt = pkt_new();
	pkt_status_code st = PKT_OK;

	st = pkt_set_type(pkt, 0);
	CU_ASSERT_EQUAL(st, E_TYPE);
	CU_ASSERT_EQUAL(pkt_get_type(pkt), 0);
	st = pkt_set_type(pkt, PTYPE_DATA);
	CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_EQUAL(pkt_get_type(pkt), PTYPE_DATA);
	st = pkt_set_type(pkt, PTYPE_ACK);
	CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_EQUAL(pkt_get_type(pkt), PTYPE_ACK);
	st = pkt_set_type(pkt, PTYPE_NACK);
	CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_EQUAL(pkt_get_type(pkt), PTYPE_NACK);

	pkt_del(pkt);
}

void test_set_tr() {
	pkt_t *pkt = pkt_new();
	pkt_status_code st = PKT_OK;
	pkt_set_type(pkt, PTYPE_DATA);

	st = pkt_set_tr(pkt, 0);
	CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_EQUAL(pkt_get_tr(pkt), 0);
	pkt_set_type(pkt, 2);
	st = pkt_set_tr(pkt, 1);
	CU_ASSERT_EQUAL(st, E_TR);
	CU_ASSERT_EQUAL(pkt_get_tr(pkt), 0);

	pkt_del(pkt);
}

void test_set_window() {
	pkt_t *pkt = pkt_new();
	pkt_status_code st = PKT_OK;

	st = pkt_set_window(pkt, 40);
	CU_ASSERT_EQUAL(st, E_WINDOW);
	CU_ASSERT_EQUAL(pkt_get_window(pkt), 0);
	st = pkt_set_window(pkt, 5);
	CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_EQUAL(pkt_get_window(pkt), 5);

	pkt_del(pkt);
}

void test_set_seqnum() {
	pkt_t *pkt = pkt_new();

	pkt_set_seqnum(pkt, 123);
	CU_ASSERT_EQUAL(pkt_get_seqnum(pkt), 123);

	pkt_del(pkt);
}

void test_set_length() {
	pkt_t *pkt = pkt_new();
	pkt_status_code st = PKT_OK;

	st = pkt_set_length(pkt, 10);
	CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_EQUAL(pkt_get_length(pkt), 10);

	st = pkt_set_length(pkt, 20);
	CU_ASSERT_EQUAL(st, PKT_OK);
	st = pkt_set_length(pkt, 600);
	CU_ASSERT_EQUAL(st, E_LENGTH);
	CU_ASSERT_EQUAL(pkt_get_length(pkt), 20);
  st = PKT_OK;
  st = pkt_set_length(pkt, 513);
  CU_ASSERT_EQUAL(st, E_LENGTH);
	CU_ASSERT_EQUAL(pkt_get_length(pkt), 20);
  st = PKT_OK;

  st = pkt_set_length(pkt, 510);
  CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_EQUAL(pkt_get_length(pkt), 510);

	pkt_del(pkt);
}

void test_set_timestamp() {
	pkt_t *pkt = pkt_new();
	pkt_status_code st = PKT_OK;

	st = pkt_set_timestamp(pkt, 42);
	CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_EQUAL(pkt_get_timestamp(pkt), 42);

	pkt_del(pkt);
}

void test_set_crc() {
	pkt_t *pkt = pkt_new();
	pkt_set_type(pkt, PTYPE_DATA);
	pkt_status_code st = PKT_OK;

	st = pkt_set_crc1(pkt, 1234);
	CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_EQUAL(pkt_get_crc1(pkt), 1234);

	/* CRC2 */

	//cas 1 : TR=0 et payload=NULL
	pkt_set_tr(pkt, 0);
	st = pkt_set_crc2(pkt, 543);
	CU_ASSERT_EQUAL(st, E_CRC);
	CU_ASSERT_EQUAL(pkt_get_crc2(pkt), 0);

	//cas 2 : TR=1 et payload=NULL
	pkt_set_tr(pkt, 1);
	st = pkt_set_crc2(pkt, 543);
	CU_ASSERT_EQUAL(st, E_CRC);
	CU_ASSERT_EQUAL(pkt_get_crc2(pkt), 0);

	//cas 3 : TR=1 et payload!=NULL
	st = pkt_set_payload(pkt, "Coucou", (uint16_t) strlen("Coucou"));
	//CU_ASSERT_PTR_NOT_NULL(pkt_get_payload(pkt));
	CU_ASSERT_EQUAL(st, PKT_OK);
	st = pkt_set_crc2(pkt, 543);
	CU_ASSERT_EQUAL(st, E_CRC);
	CU_ASSERT_EQUAL(pkt_get_crc2(pkt), 0);

	//cas 4 : TR=0 et payload!=NULL
	pkt_set_tr(pkt, 0);
	st = pkt_set_crc2(pkt, 543);
	CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_EQUAL(pkt_get_crc2(pkt), 543);

	pkt_del(pkt);
}

void test_pkt_set_payload() {
	pkt_t *pkt = pkt_new();
	pkt_status_code st = PKT_OK;

	//cas 1 : length == 7 < 512 && data == "Coucou"
	st = pkt_set_payload(pkt, "Coucou", 7);
	CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_PTR_NOT_NULL_FATAL(pkt_get_payload(pkt));
	CU_ASSERT_NSTRING_EQUAL(pkt_get_payload(pkt), "Coucou", 6);
	CU_ASSERT_EQUAL(pkt_get_length(pkt), 7);

	pkt_del(pkt);
	pkt = pkt_new();

	//cas 2 : length == 512 && data == "123..."
	char *msg = "123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789\n123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789\n123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789\n123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789\n123456789123456789123456789123456789";
	st = pkt_set_payload(pkt, msg, 512);
	CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_PTR_NOT_NULL_FATAL(pkt_get_payload(pkt));
	CU_ASSERT_NSTRING_EQUAL(pkt_get_payload(pkt), msg, 512);
	CU_ASSERT_EQUAL(pkt_get_length(pkt), 512);

	pkt_del(pkt);
	pkt = pkt_new();

	//cas 3 : length == 600 > 512 && data == "Coucou"
	st = pkt_set_payload(pkt, "Coucou", 600);
	CU_ASSERT_EQUAL(st, E_LENGTH);
	CU_ASSERT_PTR_NULL(pkt_get_payload(pkt));
	CU_ASSERT_EQUAL(pkt_get_length(pkt), 0);

	pkt_del(pkt);
	pkt = pkt_new();

	//cas 4 : length == 521 > 512 && data == "123456789..." (prend plus de 512 octets)
	char *msg_to_long = "123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789\n123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789\n123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789\n123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789\n123456789123456789123456789123456789123456789";
	st = pkt_set_payload(pkt, msg_to_long, 521);
	CU_ASSERT_EQUAL(st, E_LENGTH);
	CU_ASSERT_PTR_NULL(pkt_get_payload(pkt));
	CU_ASSERT_EQUAL(pkt_get_length(pkt), 0);

	pkt_del(pkt);
	pkt = pkt_new();

	//cas 5 : length == 500 && data == "123456789..."
	char msg_to_long_but_shorter[500] = "123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789\n123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789\n123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789\n123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789\n123456789123456789123456";
	st = pkt_set_payload(pkt, msg_to_long, 500);
	CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_PTR_NOT_NULL_FATAL(pkt_get_payload(pkt));
	CU_ASSERT_NSTRING_EQUAL(pkt_get_payload(pkt), "123456789", 9);
	CU_ASSERT_EQUAL(pkt_get_length(pkt), 500);

	pkt_del(pkt);
	pkt = pkt_new();

	//cas 6 : length == 7 < sizeof(data) < 512 && data == "abcdefghiklm"
	st = pkt_set_payload(pkt, msg_to_long_but_shorter, 7);
	CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_PTR_NOT_NULL_FATAL(pkt_get_payload(pkt));
	CU_ASSERT_NSTRING_EQUAL(pkt_get_payload(pkt), "1234567", 7);
	CU_ASSERT_EQUAL(pkt_get_length(pkt), 7);

	pkt_del(pkt);/*
	// pkt = pkt_new();
  //
	// //cas 7 : length == 8 > sizeof(data) < 512 && data == "AH"
	// // On ne peut pas tester ce cas en pratique car data peut ne pas être un string
	// // et donc avoir une longueur indéfinie
	// st = pkt_set_payload(pkt, "AH", 8);
	// CU_ASSERT_EQUAL(st, E_LENGTH);
	// CU_ASSERT_PTR_NULL(pkt_get_payload(pkt));
	// CU_ASSERT_EQUAL(pkt_get_length(pkt), 0);
  //
	// pkt_del(pkt);*/
	pkt = pkt_new();

	//cas 8 : pkt == NULL
	st = pkt_set_payload(pkt, msg_to_long_but_shorter, 7);
	CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_PTR_NOT_NULL_FATAL(pkt_get_payload(pkt));
	CU_ASSERT_NSTRING_EQUAL(pkt_get_payload(pkt), "1234567", 7);
	CU_ASSERT_EQUAL(pkt_get_length(pkt), 7);

	pkt_del(pkt);
	pkt = pkt_new();

	//cas 9 : data == NULL
	st = pkt_set_payload(pkt, NULL, 7);
	CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_PTR_NULL(pkt_get_payload(pkt));
	CU_ASSERT_EQUAL(pkt_get_length(pkt), 0);
	pkt_del(pkt);
	//pkt = pkt_new();

	// //cas 10 : length == 0
	st = pkt_set_payload(pkt, msg_to_long_but_shorter, 0);
	CU_ASSERT_EQUAL(st, PKT_OK);
	CU_ASSERT_PTR_NULL(pkt_get_payload(pkt));
	CU_ASSERT_EQUAL(pkt_get_length(pkt), 0);
  //
	// pkt_del(pkt);
}

void test_varuint_len() {
	size_t ans = 0;
  uint8_t *data_short = (uint8_t*) calloc(sizeof(uint8_t),1);
  if(data_short == NULL) return;
  uint8_t *data_long = (uint8_t*) calloc(sizeof(uint8_t) * 2,1);
  if(data_long == NULL) return;

  uint16_t val = 0;
  // Case 1 : length de 0, donc L=0
  memcpy(data_short,&val,1);
  ans = varuint_len(data_short);
  CU_ASSERT_EQUAL(ans,1);
  // Case 2 : length de 2, donc L=0
  val = 2;
  memcpy(data_short,&val,1);
  ans = varuint_len(data_short);
  CU_ASSERT_EQUAL(ans,1);
  // Case 3 : length de 80, donc L=0 (les bits donnent 1000 0000)
  val = 80;
  memcpy(data_short,&val,1);
  ans = varuint_len(data_short);
  CU_ASSERT_EQUAL(ans,1);
  // Case 4 : length de 256, donc L=1 (les bits donnent 1000 0001 0000 0000)
  val = htons(33024);
  memcpy(data_long,&val,2);
  ans = varuint_len(data_long);
  CU_ASSERT_EQUAL(ans,2);
  // Case 5 : length de 384, donc L=1 (les bits donnent 1000 0001 1000 0000)
  val = htons(33152);
  memcpy(data_long,&val,2);
  ans = varuint_len(data_long);
  CU_ASSERT_EQUAL(ans,2);
  // Case 6 : length de 8192, donc L=1
  val = htons(40960);
  memcpy(data_long,&val,2);
  ans = varuint_len(data_long);
  CU_ASSERT_EQUAL(ans,2);
  // Fin
  free(data_short);
  free(data_long);
}

void test_varuint_predict_len() {
	ssize_t ans = 0;
  uint16_t testValue = 0;

  // Case 1 : val = 0, devrait renvoyer 1
  testValue = 0;
  ans = varuint_predict_len(testValue);
  CU_ASSERT_EQUAL(ans,1);
  // Case 2 : val = 42, devrait renvoyer 1
  testValue = 42;
  ans = varuint_predict_len(testValue);
  CU_ASSERT_EQUAL(ans,1);
  // Case 3 : val = 512, devrait renvoyer 2
  testValue = 512;
  ans = varuint_predict_len(testValue);
  CU_ASSERT_EQUAL(ans,2);
  // Case 4 : val = 0x8000, devrait renvoyer -1
  testValue = 0x8000;
  ans = varuint_predict_len(testValue);
  CU_ASSERT_EQUAL(ans,-1);
  // Case 5 : val = 0xC000, devrait renvoyer -1
  testValue = 0xC000;
  ans = varuint_predict_len(testValue);
  CU_ASSERT_EQUAL(ans,-1);
}

void test_predict_header_length() {
  pkt_t *pkt = pkt_new();
	ssize_t ans = 0;

  // Case 1 : length = 0, devrait renvoyer 11
  pkt->length = 0;
  ans = predict_header_length(pkt);
  CU_ASSERT_EQUAL(ans,11);
  // Case 2 : length = 42, devrait renvoyer 11
  pkt->length = 42;
  ans = predict_header_length(pkt);
  CU_ASSERT_EQUAL(ans,11);
  // Case 3 : length = 512, devrait renvoyer 12
  pkt->length = 512;
  ans = predict_header_length(pkt);
  CU_ASSERT_EQUAL(ans,12);
  // Case 4 : length = 0x8000, devrait renvoyer -1
  pkt->length = 0x8000;
  ans = predict_header_length(pkt);
  CU_ASSERT_EQUAL(ans,-1);
  // Case 5 : length = 0xC000, devrait renvoyer -1
  pkt->length = 0xC000;
  ans = predict_header_length(pkt);
  CU_ASSERT_EQUAL(ans,-1);

  pkt_del(pkt);
}

void test_varuint_decode() {
	size_t ans = 1;
  CU_ASSERT_FALSE(ans);
}

void test_varuint_encode() {
	ssize_t ans = 1;
  uint16_t val = 1;
  uint8_t *data_short = (uint8_t*) calloc(sizeof(uint8_t),1);
  if(data_short == NULL) return;
  uint8_t *data_long = (uint8_t*) calloc(sizeof(uint8_t) * 2,1);
  if(data_long == NULL) return;
  // Cas 1 : val = 1; data = 1, ans = 1
  val = 1;
  ans = varuint_encode(val, data_short, 1);
  CU_ASSERT_EQUAL(ans,1);
  CU_ASSERT_EQUAL(*data_short,1);
  // Cas 2 : val = 127; data = 127, ans = 1
  val = 127;
  ans = varuint_encode(val, data_short, 1);
  CU_ASSERT_EQUAL(ans,1);
  CU_ASSERT_EQUAL(*data_short,127);
  // Cas 3 : val = 129; data = , ans : 2
  // Case 3 : val = 256; data = 1, ans = 2
  val = 256;
  ans = varuint_encode(val, data_long, 2);
  CU_ASSERT_EQUAL(ans,2);
  CU_ASSERT_EQUAL(*data_long,1);
  // Fin
  free(data_short);
  free(data_long);
}

void test_encode_decode() {
    pkt_t *pkt = pkt_new();
    pkt_t *recv_pkt = pkt_new();
    pkt_set_type(pkt,PTYPE_DATA);
    pkt_set_payload(pkt, "COUCOU", 7);

    pkt_status_code st;
    size_t payloadLength = 20;

    char *buf = (char*) malloc(sizeof(char) * payloadLength);
    char *buf2 = (char*) malloc(sizeof(char) * payloadLength);
    st = pkt_encode(pkt, buf, &payloadLength);
    CU_ASSERT_EQUAL(st, PKT_OK);
    st = pkt_decode(buf2, 20, recv_pkt);
    CU_ASSERT_EQUAL(st, E_TYPE);
    CU_ASSERT_EQUAL(pkt_get_type(pkt),pkt_get_type(recv_pkt));
    CU_ASSERT_EQUAL(pkt_get_length(pkt),pkt_get_length(recv_pkt));
    //CU_ASSERT_NSTRING_EQUAL(pkt_get_payload(pkt),pkt_get_payload(recv_pkt),10);
    CU_ASSERT_EQUAL(pkt_get_crc1(pkt),pkt_get_crc1(recv_pkt));
    CU_ASSERT_EQUAL(pkt_get_crc2(pkt),pkt_get_crc2(recv_pkt));
    pkt_del(pkt);
}
