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

general_status_code update_seqnum(uint8_t *old_seqnum) {
  *old_seqnum = (*old_seqnum + 1) % 256;
  return OK;
}

void changeWindow(int window, int * curLow, int *curHi, bool * curWindow) {

  //Gestion des erreurs 
  if(*curLow < 0 || *curLow > 255 || *curHi < 0 || *curHi > 255 || window < 0 || window > 255){
    printf("Erreur d'indice ");
    return;
  }

  //Recupération de l'écart entre curLow et currHi
  int diff;
  if (*curLow < *curHi){
		diff = *curHi - *curLow ;
  } else {
    diff = *curLow - *curHi ;
  }
  
  //Si on doit augmenter la fenêtre
  if(window > diff){
    int diff2 = window - diff;
    for(int i = 0;i<diff2;i++){
      *curHi = (*curHi + 1) %256; //Augmentation de curHi, si >255 revient à 0
    }

  //Si on doit diminuer la fenêtre    
  } else {
    int diff2 = diff - window;
    for(int i = 0;i<diff2;i++){
      curWindow[*curHi] = false;
      *curHi = ((*curHi - 1) + 256) %256; //Augmentation de curHi, si <0 revient à 255
    }
	}
  
}