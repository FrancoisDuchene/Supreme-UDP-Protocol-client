#ifndef _GENERAL_H_
#define _GENERAL_H_

typedef enum {
  OK,         // SUCCÈS, tout va bien
  E_NOMEMORY,    // Mémoire insuffisante
  E_POLL,     // Erreur liée à POLL
  E_TIMEOUT,  // Erreur de timeout, pas de connexion
  E_DECODE,   // Erreur lié au décodage du paquet reçu, erreur fatale ne pouvant être résolue
  E_ENCODE,   // Erreur lié à l'encodage du paquet à envoyer, erreur fatale ne pouvant être résolue
  E_SEQNUM_GEN, // Erreur lié au seqnum du paquet
  E_INCOHERENT, // Erreur d'incohérence
  E_BUILD,    // Erreur lors de la construction du paquet
  E_PKT_QUEUE,  // Erreur dans la file de retransmission des paquets
} general_status_code;

#endif
