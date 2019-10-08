#include "test.h"

int main(void) {
  CU_pSuite pSuite_format = NULL;

  //init
  if(CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  // test pour le format
  pSuite_format = CU_add_suite("Implémentation format paquets", NULL, NULL);
  if(pSuite_format == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  if((CU_add_test(pSuite_format, "Test pkt_new", test_pkt_new) == NULL) ||
      CU_add_test(pSuite_format, "Test pkt_getters", test_pkt_getters) == NULL ||
      CU_add_test(pSuite_format, "Test pkt_set_type", test_set_type) == NULL ||
      CU_add_test(pSuite_format, "Test pkt_set_tr", test_set_tr) == NULL ||
      CU_add_test(pSuite_format, "Test pkt_set_window", test_set_window) == NULL ||
      CU_add_test(pSuite_format, "Test test_set_seqnum", test_set_seqnum) == NULL ||
      CU_add_test(pSuite_format, "Test test_set_length", test_set_length) == NULL ||
      CU_add_test(pSuite_format, "Test test_set_timestamp", test_set_timestamp) == NULL ||
      CU_add_test(pSuite_format, "Test test_set_crc", test_set_crc) == NULL ||
      CU_add_test(pSuite_format, "Test test_pkt_set_payload", test_pkt_set_payload) == NULL ||
      CU_add_test(pSuite_format, "Test test_varuint_len", test_varuint_len) == NULL ||
      CU_add_test(pSuite_format, "Test test_varuint_predict_len", test_varuint_predict_len) == NULL ||
      CU_add_test(pSuite_format, "Test test_predict_header_length", test_predict_header_length) == NULL ||
      CU_add_test(pSuite_format, "Test test_varuint_decode", test_varuint_decode) == NULL ||
      CU_add_test(pSuite_format, "Test test_varuint_encode", test_varuint_encode) == NULL ||
      CU_add_test(pSuite_format, "Test test_encode_decode", test_encode_decode) == NULL
    )
  {
    CU_cleanup_registry();
    return CU_get_error();
  }

  //run
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_basic_show_failures(CU_get_failure_list());

  fprintf(stderr, "\n");
  CU_cleanup_registry();

  return CU_get_error();
}
