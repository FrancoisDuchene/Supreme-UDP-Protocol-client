#ifndef _GENERAL_H_
#define _GENERAL_H_

typedef enum {
  OK,         // SUCCÈS, tout va bien
  E_NOMEM,    // Mémoire insuffisante
  E_POLL,     // Erreur liée à POLL
  E_TIMEOUT,  // Erreur de timeout, pas de connexion
  E_DECODE,   // Erreur lié au décodage du paquet reçu, erreur fatale ne pouvant être résolue
  E_ENCODE,   // Erreur lié à l'encodage du paquet envoyé, erreur fatale ne pouvant être résolue
} general_status_code;

#endif
