#include <stdlib.h>
#include <stdint.h>

#include "general.h"
#include "packet_interface.h"

general_status_code long_builder_pkt(pkt_t *pkt, ptypes_t type, uint8_t tr, uint8_t window, 
                uint8_t seqnum, uint32_t timestamp, char *payload, uint16_t payload_length);

general_status_code short_builder_pkt(pkt_t *pkt, ptypes_t type, uint8_t tr, uint8_t window, 
            uint8_t seqnum);