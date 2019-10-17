#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

#include "general.h"
#include "packet_interface.h"

/* Fonction qui check si le seqnum donné se trouve dans la window désigné par les
 * deux marqueurs cur_debut et cur_fin qui indiquent respectivement le début et la fin
 * de la window utilisée
 * @return: true si le seqnum se trouve dans la window, false sinon
 */
bool is_in_window(uint8_t cur_debut, uint8_t cur_fin, uint8_t seqnum);

#endif
