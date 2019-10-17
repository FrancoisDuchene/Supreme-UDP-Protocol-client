#ifndef __READ_WRITE_LOOP_H_
#define __READ_WRITE_LOOP_H_

#include <stdlib.h> /* EXIT_X */
#include <stdio.h> /* fprintf */
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdbool.h>

#include "general.h"
#include "packet_interface.h"

#define TIMEOUT -1

/* Loop reading a socket and printing to stdout,
 * while reading stdin and writing to the socket
 * @sfd: The socket file descriptor. It is both bound and connected.
 * @return: as soon as stdin signals EOF, and a general_status_code indicating
 *          the success or not of the operation
 */
general_status_code read_write_loop(const int sfd);

/* Libère les ressources allouées. Les arguments peuvent être égal à nul.
 * Cette fonction doit être appellée avant chaque return à la fonction principale
 */
general_status_code free_loop_res(char *buffer, pkt_t *pkt, int * curLow, int *curHi, bool* curWindow, int * curWindowSize);

void pkt_Ack(int seqnum);

void pkt_Nack(int seqnum);

#endif
