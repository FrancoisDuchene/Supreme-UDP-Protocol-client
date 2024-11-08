#include <stdlib.h> /* EXIT_X */
#include <stdio.h> /* fprintf */
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "read_write_loop.h"

#define TIMEOUT -1

/* Loop reading a socket and printing to stdout,
 * while reading stdin and writing to the socket
 * @sfd: The socket file descriptor. It is both bound and connected.
 * @return: as soon as stdin signals EOF
 */
void read_write_loop(int sfd){
	char *buf = (char *) malloc(1024*sizeof(char));
	if(buf == NULL) {
		perror("Erreur malloc read_write_loop");
		return;
	}

	int eof_stdin = 1;
	int eof_sfd = 1;
	int status;

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
			return;
		} else if (status == 0) {
			fprintf(stderr, "Timeout has occured ! No data transfered after %d seconds\n", TIMEOUT);
			return;
		} else {
				if (ufds[0].revents & POLLIN && eof_stdin) {
					// On lit sur l'input et on l'envoie
		      size_t readLen = read(STDIN_FILENO, buf, sizeof(buf));
					readLen = send(sfd, (void *) buf, readLen, 0);
					if(readLen == 0) {
						eof_stdin = 0;
					}

				}

				if (ufds[1].revents & POLLIN && eof_sfd) {
					// On reçoit un message et on l'affiche
					size_t readLen = recv(sfd, (void *) buf, 1024, 0);
					readLen = write(STDOUT_FILENO, (void *) buf, readLen);
					if(!readLen)
						eof_sfd = 0;
				}
		}
  }
	free(buf);
}
