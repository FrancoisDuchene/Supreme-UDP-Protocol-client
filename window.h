#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

#include "general.h"
#include "packet_interface.h"

general_status_code update_seqnum(uint8_t *old_seqnum);

/* Fonction permettant de redimensionner la taille de la window active
 * Modifie curHi ainsi que les cases de la window concernées
 * Print erreur si le seqnum, curLow et curHi n'ont pas de valeurs cohérentes 
 */
void changeWindow(int seqnum,int *curLow,int *curHi);

#endif
