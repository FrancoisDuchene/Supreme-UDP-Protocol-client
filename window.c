#include "window.h"

bool is_in_window(uint8_t cur_debut, uint8_t cur_fin, uint8_t seqnum) {
  if(cur_fin < cur_debut) {
    if (cur_debut < seqnum < 256 && -1 < seqnum < cur_fin)
      return true;
    else
      return false;
  } else {
    if ((cur_debut - seqnum) > cur_debut)
      return true;
    else
      return false;
  }
}
