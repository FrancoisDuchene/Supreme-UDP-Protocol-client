#include "read_write_loop_final.h"
#include "packet_interface.h"

/* Loop reading a socket and printing to stdout,
 * while reading stdin and writing to the socket
 * @sfd: The socket file descriptor. It is both bound and connected.
 * @return: as soon as stdin signals EOF
 */
general_status_code read_write_loop(int sfd) {
	// Variables utilisees durant l'exécution
	pkt_status_code status;
	char *buf = (char *) malloc(1024*sizeof(char));
	if(buf == NULL) {
		perror("Erreur malloc read_write_loop");
		return E_NOMEM;
	}

	int eof_stdin = 1;
	int eof_sfd = 1;
	int status;
	pkt_t *pkt_actu = NULL;

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
			perror("An error has occured with poll");
			free_loop_res(buf, pkt_actu);
			return E_POLL;
		} else if (status == 0) {
			fprintf(stderr, "Timeout has occured ! No data transfered after %d seconds\n", TIMEOUT);
			free_loop_res(buf, pkt_actu);
			return E_TIMEOUT;
		} else {
			if (ufds[0].revents & POLLIN && eof_stdin) {
				// On lit sur l'input et on l'envoie
				size_t readLen = read(STDIN_FILENO, buf, sizeof(buf));

				pkt_actu = pkt_new();
				if(pkt_actu == NULL){
					print("Erreur allocation du paquet \n");
					free_loop_res(buf, pkt_actu);
					return E_NOMEM;
				}

				//To do, set dans le paquet ce qu'on a récupéré dans le buf


				/*
				char *new_buf = (char *) malloc(readLen);
				if(new_buf == NULL) {
					perror("Erreur malloc new_buf read_write_loop");
					free_loop_res(buf, pkt_actu);
					return E_NOMEM;
				} */

				status = pkt_encode(pkt_actu,buf,readLen);

				if(status != PKT_OK ){
					print("Erreur lors du encode de type : %u\n",status);
					free_loop_res(buf, pkt_actu);
					return E_ENCODE;
				}

				readLen = send(sfd, (void *) pkt_actu, readLen, 0);
				if(readLen == 0) {
					eof_stdin = 0;
				}

			}

			if (ufds[1].revents & POLLIN && eof_sfd) {
				// On reçoit un message et on l'affiche
				size_t readLen = recv(sfd, (void *) buf, 1024, 0);

				pkt_actu = pkt_new();
				if(pkt_actu == NULL){
					print("Erreur allocation du paquet \n");
					free_loop_res(buf, pkt_actu);
					return E_NOMEM;
				}

				status = pkt_decode(buf,readLen,pkt_actu);
				if(status != 0 ) {
					print("Erreur lors du decode de type : %u\n",status);
					free_loop_res(buf, pkt_actu);
					return E_DECODE;
				}

				int type = pkt_get_type ;

				if (type ==  PTYPE_DATA){
					print("Lol mais on est pas un receiver ici, vous êtes toctoc, fufu \n");
				} else if (type == PTYPE_ACK) {
					pkt_Ack();
				} else if(type == PTYPE_NACK) {
					pkt_Nack();
				} else {
					print("Ce type est inconnu au bataillon \n");
				}



				readLen = write(STDOUT_FILENO, (void *) buf, readLen);
				if(!readLen)
					eof_sfd = 0;
			}
		}
  	}
	free_loop_res(buf, pkt_actu);
	return OK;
}

general_status_code free_loop_res(char *buffer, pkt_t *pkt) {
	if(buffer != NULL) free(buffer);
	if(pkt != NULL) pkt_del(pkt);
	return OK;
}

void pkt_Ack() {
	return;
}

void pkt_Nack() {
	return;
}