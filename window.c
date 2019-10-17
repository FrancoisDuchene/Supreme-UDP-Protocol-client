#include "window.h"

bool is_in_window(uint8_t cur_debut, uint8_t cur_fin, uint8_t seqnum) {
  if(cur_fin < cur_debut) {
    if (cur_debut < seqnum && seqnum < cur_fin)
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

general_status_code update_seqnum(pkt_t *pkt, uint8_t old_seqnum) {
  if(old_seqnum == 255) {
    old_seqnum = 0; 
  } else {
    old_seqnum++;
  }
  if(pkt_set_seqnum(pkt, old_seqnum) != PKT_OK) {
    return E_UPDATESEQNUM;
  }
  return OK;
}