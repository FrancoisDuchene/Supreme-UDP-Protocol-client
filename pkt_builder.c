#include "pkt_builder.h"

general_status_code long_builder_pkt(pkt_t *pkt, ptypes_t type, uint8_t tr, uint8_t window, 
                uint8_t seqnum, uint32_t timestamp, char *payload, uint16_t payload_length) {
    if( pkt_set_type(pkt, type) != PKT_OK) {
        fprintf(stderr, "Error encode - set_type");
        return E_BUILD;
    }
    if( pkt_set_tr(pkt, tr) != PKT_OK) {
        fprintf(stderr, "Error encode - set_tr");
        return E_BUILD;
    }
    if( pkt_set_window(pkt, window) != PKT_OK) {
        fprintf(stderr, "Error encode - set_window");
        return E_BUILD;
    }
    if( pkt_set_seqnum(pkt, seqnum) != PKT_OK) {
        fprintf(stderr, "Error encode - set_seqnum");
        return E_BUILD;
    }
    if (pkt_set_timestamp(pkt, timestamp) != PKT_OK) {
        fprintf(stderr, "Error encode - set_timestamp");
        return E_BUILD;
    }
    if( pkt_set_payload(pkt, payload, payload_length) != PKT_OK) {
        fprintf(stderr, "Error encode - set_payload");
        return E_BUILD;
    }
    return OK;
}

general_status_code short_builder_pkt(pkt_t *pkt, ptypes_t type, uint8_t tr, uint8_t window, 
            uint8_t seqnum) {
    return long_builder_pkt(pkt, type, tr, window, seqnum, 0, NULL, 0);
}