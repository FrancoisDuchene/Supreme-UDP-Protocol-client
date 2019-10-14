#include <stdio.h> /* fprintf */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "wait_for_client.h"

/* Block the caller until a message is received on sfd,
 * and connect the socket to the source addresse of the received message
 * @sfd: a file descriptor to a bound socket but not yet connected
 * @return: 0 in case of success, -1 otherwise
 * @POST: This call is idempotent, it does not 'consume' the data of the message,
 * and could be repeated several times blocking only at the first call.
 */
int wait_for_client(int sfd) {
	#define BUF_SIZE 1024
	char buf[BUF_SIZE];
	struct sockaddr_in6 fromAddr;
	socklen_t fromAddrLen = sizeof(fromAddr);
	ssize_t bytes_recv = recvfrom(sfd, buf, BUF_SIZE, MSG_PEEK,
																  (struct sockaddr*) &fromAddr, &fromAddrLen);
	if (bytes_recv < 0) {
		fprintf(stderr, "No bytes received\n");
		return -1;
	} else {
		if(connect(sfd, (struct sockaddr *) &fromAddr, fromAddrLen) == -1) {
			fprintf(stderr, "Try to connect received socket failed\n");
			return -1;
		}
		return 0;
	}
}
