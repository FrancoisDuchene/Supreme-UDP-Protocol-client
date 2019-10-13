#include <stdlib.h> /* EXIT_X */
#include <stdio.h> /* fprintf */
#include <sys/select.h>
#include <netdb.h>
#include <string.h>

#include "read_write_loop.h"

/* Loop reading a socket and printing to stdout,
 * while reading stdin and writing to the socket
 * @sfd: The socket file descriptor. It is both bound and connected.
 * @return: as soon as stdin signals EOF
 */
void read_write_loop(int sfd){
	fd_set readsfd;
	fd_set writesfd;
	struct timeval timeout;
	char buf[1024];
	char buf2[1024];

	timeout.tv_sec = 3;
	timeout.tv_usec = 500000;

	FD_ZERO(&readsfd);
 	FD_ZERO(&writesfd);
	FD_SET(sfd, &readsfd);
	FD_SET(sfd, &writesfd);

	size_t endOfStream = 1;

	while (endOfStream) { // When EOF is reached, fread returns 0

		if (select(sfd + 1, &readsfd, &writesfd, NULL, &timeout) == -1) {
			printf (" Error select \n");
			return ;
		}

		endOfStream = fread(buf, strlen(buf)+1, 1, stdin);
		if (buf != NULL) {
			if(FD_ISSET(sfd, &writesfd)){
				if(send(sfd,(void *) buf,sizeof(buf),0)==-1){
					printf(" Error recv \n");
					return;
				}
			}
		}

		if(FD_ISSET(sfd, &readsfd)){
			if(recv(sfd,(void*)buf2,sizeof(buf2),0)==-1){
				printf(" Error recv \n");
				return;
			}

			if(strcmp(buf , "Q") == 0){
				printf("End\n");
                break;
			}
			fwrite(buf2, strlen(buf2)+1, 1, stdout);
		}

	}
}
