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

void changeWindow(int window,int * curLow,int *curHi,bool * curWindow) {

  //Gestion des erreurs 
  if(*curLow < 0 || *curLow > 255 || *curHi< 0 || *curHi > 255 || window < 0 || window > 255){
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
  
  int diff2;
  //Si on doit augmenter la fenêtre
  if(window > diff){
    diff2 = window - diff;
    for(int i = 0;i<diff2;i++){
      window[curHi] = false;
      *curHi = (*curHi + 1) %256;
    }

  //Si on doit diminuer la fenêtre    
  } else {
    diff2 = diff - window;
    for(int i = 0;i<diff2;i++){
      window[curHi] = false;
      *curHi = (*curHi - 1) %256;
      
    }

	}

}