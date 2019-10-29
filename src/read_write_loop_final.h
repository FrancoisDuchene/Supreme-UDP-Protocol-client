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
#include <time.h>

#include "general.h"
#include "packet_interface.h"
#include "window.h"
#include "pkt_builder.h"
#include "linked_list.h"

#define TIMEOUT 3000
/**
 * Loop reading a socket and printing to stdout,
 * while reading stdin and writing to the socket
 * @sfd: The socket file descriptor. It is both bound and connected.
 * @fd: The file descriptor on input
 * @return: as soon as stdin signals EOF, and a general_status_code indicating
 *          the success or not of the operation
 */
general_status_code read_write_loop(const int sfd, const int fd);

/** Encode un paquet dont la taille est définie par readLen dans le buffer et l'envoie
 *  sur le socket sfd
 * @return: OK si tout s'est bien passé ou bien E_ENCODE si une erreur est survenue lors de l'encodage du paquet
 */
general_status_code send_packet(int sfd, pkt_t* pkt, char *buffer, size_t *readLen);

/** Libère les ressources allouées. Les arguments peuvent être égal à nul.
 * Cette fonction doit être appellée avant chaque return à la fonction principale
 */
general_status_code free_loop_res(char *buffer, char *buffer_read, pkt_t *pkt, pkt_t *pkt2, 
    int * curLow, int *curHi, struct pktList* curPktList, uint8_t *actual_seqnum, size_t *readLen);

/** Fonction gérant la réception de acks
 * Deplace les indices de la fenêtre active en fonction du seqnum reçu
 * Return erreur si seqnum invalide 
 */
general_status_code pkt_Ack(int seqnum, int * curLow,int *curHi, struct pktList* curPktList);

/** Fonction gérant la réception de nacks
 * Return erreur si seqnum invalide 
 */
general_status_code pkt_Nack(int seqnum,int * curLow,int *curHi, struct pktList* curPktList);

#endif
