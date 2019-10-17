#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/types.h>

#include "packet_interface.h"

void print_pkt_osef(pkt_t* pkt, bool print_payload);

#endif