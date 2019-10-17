#include "pkt_builder.h"

general_status_code long_builder_pkt(pkt_t *pkt, ptypes_t type, uint8_t tr, uint8_t window, 
                uint8_t seqnum, uint32_t timestamp, char *payload, uint16_t payload_length) {
    pkt_set_type(pkt, type);
    pkt_set_tr(pkt, tr);
    pkt_set_window(pkt, window);
    pkt_set_seqnum(pkt, seqnum);
    pkt_set_timestamp(pkt, timestamp);
    pkt_set_payload(pkt, payload, payload_length);
    return OK;
}

general_status_code short_builder_pkt(pkt_t *pkt, ptypes_t type, uint8_t tr, uint8_t window, 
            uint8_t seqnum) {
    return long_builder_pkt(pkt, type, tr, window, seqnum, 0, NULL, 0);
}