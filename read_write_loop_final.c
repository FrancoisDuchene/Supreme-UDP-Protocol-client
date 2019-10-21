#include "read_write_loop_final.h"


int nbCurPkt = 0;

/* Loop reading a socket and printing to stdout,
 * while reading stdin and writing to the socket
 * @sfd: The socket file descriptor. It is both bound and connected.
 * @return: as soon as stdin signals EOF
 */
general_status_code read_write_loop(int sfd) {

	//La table window et ses indices
	int * curLow = (int *) malloc(sizeof(int));
	if (curLow == NULL){
		perror("Erreur malloc indice1\n");
		return E_NOMEMORY;
	}
	*curLow = 0;

	int * curHi = (int *) malloc(sizeof(int));;
	if (curHi == NULL){
		perror("Erreur malloc indice2\n");
		free(curLow);
		return E_NOMEMORY;
	}
	*curHi = 1;

	int curWindowSize = 1;

	//Liste des paquets envoyés
	struct pktList* curPktFirst = NULL;
	struct pktList* curPktList = curPktFirst;

	// Variables utilisees durant l'exécution
	char *buf = (char *) malloc(1024*sizeof(char));
	if(buf == NULL) {
		perror("Erreur malloc read_write_loopn\n");
		free(curLow);
		free(curHi);
		return E_NOMEMORY;
	}
	// Buffer pour lire des données sur stdin
	// Si on limite à 512, le read lit automatiquement en plusieurs fois
	char *buf_read = (char *) malloc(512*sizeof(char));
	if(buf == NULL) {
		perror("Erreur malloc read_write_loop\n");
		free(curLow);
		free(curHi);
		free(buf);
		return E_NOMEMORY;
	}

	int eof_stdin = 1;
	int eof_sfd = 1;
	int status;
	pkt_status_code pkt_status;
	general_status_code gen_status;

	pkt_t *pkt_actu = pkt_new();
	if(pkt_actu == NULL) {
		fprintf(stderr, "Erreur allocation du paquet\n");
		free_loop_res(buf, pkt_actu, curLow, curHi, curPktFirst);
		return E_NOMEMORY;
	}

	uint8_t *actual_seqnum = (uint8_t*) malloc(sizeof (uint8_t));
	if(actual_seqnum == NULL) {
		perror("Erreur malloc read_write_loop\n");
		return E_NOMEMORY;
	}
	*actual_seqnum = 0;

	/* Variables lié à POLL */
	struct pollfd ufds[2];
	// Le clavier
	ufds[0].fd = STDIN_FILENO;
	ufds[0].events = POLLIN;
	// Socket d'envoi
	ufds[1].fd = sfd;
	ufds[1].events = POLLIN;

	while (eof_stdin || eof_sfd) {

		status = poll(ufds, 2, TIMEOUT);

		if(status < 0) {

			perror("An error has occured with poll\n");
			free_loop_res(buf, pkt_actu,curLow,curHi,curPktFirst);
			return E_POLL;

		} else if (status == 0) {

			fprintf(stderr, "Timeout has occured ! No data transfered after %d seconds\n", TIMEOUT);
			free_loop_res(buf, pkt_actu,curLow,curHi,curPktFirst);
			return E_TIMEOUT;

		} else {
			if (ufds[0].revents & POLLIN && eof_stdin && nbCurPkt < curWindowSize) {
				// On lit sur l'input et on l'envoie
				size_t *readLen = (size_t *) malloc(sizeof(size_t));
				if(readLen == NULL) return E_NOMEMORY;
				
				*readLen = read(STDIN_FILENO, buf_read, 512);

				gen_status = update_seqnum(actual_seqnum);
				if(gen_status != OK) {
					printf("oh mince\n");
				}
				//TODO changer la valeur de window
				
				gen_status = long_builder_pkt(pkt_actu, PTYPE_DATA, 0, 1, *actual_seqnum, 0, buf_read, *readLen);



				curPktList = (struct pktList *) malloc(sizeof(struct pktList *));
				curPktList->currentPkt = pkt_actu;

				pkt_status = pkt_encode(pkt_actu,buf, readLen);
				if(pkt_status != PKT_OK ){
					printf("Erreur lors du encode de type : %u\n", pkt_status);
					free_loop_res(buf, pkt_actu, curLow, curHi, curPktFirst);
					return E_ENCODE;
				}

				*readLen = send(sfd, (void *) buf, *readLen, 0);
				if(*readLen == 0) {
					eof_stdin = 0;
				}

				nbCurPkt = nbCurPkt + 1;
				curPktList->next = NULL;
				curPktList = curPktList->next;
				//To do, faut aussi gérer le time et donc le réenvoi de paquets qui ont posé problème
			}

			if (ufds[1].revents & POLLIN && eof_sfd) {
				//TODO gérer le EOF reçu (end of connexion)
				// On reçoit un message et on l'affiche
				size_t readLen = recv(sfd, (void *) buf, 1024, 0);

				fprintf(stderr, "Reçu %zu bytes ! \t", readLen);

				pkt_actu = pkt_new();
				if(pkt_actu == NULL){
					printf("Erreur allocation du paquet \n");
					free_loop_res(buf, pkt_actu,curLow,curHi,curPktFirst);
					return E_NOMEMORY;
				}

				pkt_status = pkt_decode(buf,readLen,pkt_actu);
				if(pkt_status != 0 ) {
					printf("Erreur lors du decode de type : %u\n",pkt_status);
					free_loop_res(buf, pkt_actu,curLow,curHi,curPktFirst);
					return E_DECODE;
				}

				int type = pkt_get_type(pkt_actu) ;

				//Verification du type de paquet 
				if (type ==  PTYPE_DATA){
					printf("Lol mais n'on est pas un receiver ici, vous êtes toctoc, fufu \n");
				} else if (type == PTYPE_ACK) {

					//vérification si tr valide
					int tr = pkt_get_tr(pkt_actu) ;
					if (tr == 0){

						//Appel de la méthode pour modifier les indices de la window en fonctions des acks reçus
						int seqnum = pkt_get_seqnum(pkt_actu);
						pkt_Ack(seqnum,curLow,curHi,curPktFirst);

						//Changement éventuel de la taille de la window
						int window = pkt_get_window(pkt_actu);
						if(window != curWindowSize){
							changeWindow(window,curLow,curHi);
							curWindowSize = window;
						}
						
					}
					
				} else if(type == PTYPE_NACK) {

					int tr = pkt_get_tr(pkt_actu) ;
					if (tr == 0){

						//Appel de la méthode gérant les nacks
						int seqnum = pkt_get_seqnum(pkt_actu);
						pkt_Nack(seqnum,curLow,curHi,curPktFirst);

					}

				} else {
					printf("Ce type est inconnu au bataillon \n");
				}

			}
		}
  	}
	free_loop_res(buf, pkt_actu,curLow,curHi,curPktFirst);
	return OK;
}

general_status_code free_loop_res(char *buffer, pkt_t *pkt, int * curLow, int *curHi, struct pktList* curPktFirst) {
	if(buffer != NULL) free(buffer);
	if(pkt != NULL) pkt_del(pkt);
	if(curLow != NULL) free(curLow);
	if(curHi != NULL) free(curHi);

	struct pktList* actu = curPktFirst;
	while(actu->next != NULL){
		curPktFirst = curPktFirst->next;
		free(actu->currentPkt);
		free(actu);
		actu = curPktFirst;
	}
	
	return OK;
}


general_status_code pkt_Ack(int seqnum, int * curLow, int *curHi, struct pktList* curPktFirst) {

	//Si le seqnum a une valeur invalide
	if (seqnum < 0 || seqnum > 255) {
		printf("Numero de seqnum invalide\n");
		return E_SEQNUM_GEN;
	} 

	//Cas 1: l'indice de début de window est plus petit que l'indice de fin de window
	if (*curLow < *curHi){

		//Si le seqnum a une valeur invalide
		if (seqnum < *curLow || seqnum > *curHi) {
			printf("Numero de seqnum invalide\n");
			return E_SEQNUM_GEN;
		} 
	//Cas 2: l'indice de début de window est plus grand que l'indice de fin de window (curHi a dépassé 255 et a donc repris au début)
	} else {

		//Si le seqnum a une valeur invalide
		if (seqnum < *curLow && seqnum > *curHi) {
			printf("Numero de seqnum invalide\n");
			return E_SEQNUM_GEN;
		} 
	}

	//Déplacement des indices de la window
	while(*curLow != seqnum){
      	*curLow = (*curLow + 1) %256;
		*curHi = (*curHi + 1) %256;
		nbCurPkt = nbCurPkt -1;
	}

	struct pktList* actu = curPktFirst;
	if (actu == NULL){
		printf("Quelque chose d'incohérent s'est produit\n");
		return  E_INCOHERENT;
	}

	//On libère tous les paquets dont le seqnum précède celui du ack reçu 
	while(actu->currentPkt->seqnum != seqnum){
		curPktFirst = curPktFirst->next;
		free(actu->currentPkt);
		free(actu);
		actu = curPktFirst;
		if (actu == NULL){
			break;
		}
	}

	return OK;
}


general_status_code pkt_Nack(int seqnum,int * curLow,int *curHi, struct pktList* curPktFirst) {
	*curPktFirst = *curPktFirst;
	//Si le seqnum a une valeur invalide
	if (seqnum < 0 || seqnum > 255) {
		printf("Numero de seqnum invalide\n");
		return E_SEQNUM_GEN;
	} 

	//Cas 1: l'indice de début de window est plus petit que l'indice de fin de window
	if (*curLow < *curHi){

		//Si le seqnum a une valeur valide
		if (seqnum > *curLow && seqnum < *curHi) {
			printf("Numero de seqnum valide, mais osef\n");

		//Si le seqnum a une valeur invalide
		} else {
			printf("Numero de seqnum invalide\n");
			return E_SEQNUM_GEN;
		}	

	//Cas 2: l'indice de début de window est plus grand que l'indice de fin de window (curHi a dépassé 255 et a donc repris au début)
	} else {

		//Si le seqnum a une valeur valide
		if ( (seqnum > *curLow && seqnum < 256) || (seqnum < *curHi && seqnum >= 0) ) {
			printf("Numero de seqnum valide, mais osef\n");

		//Si le seqnum a une valeur invalide
		} else {
			printf("Numero de seqnum invalide\n");
			return E_SEQNUM_GEN;
		}	

	}
	return OK;
}