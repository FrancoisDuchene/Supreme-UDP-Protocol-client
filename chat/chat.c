#include <stdlib.h> /* EXIT_X */
#include <stdio.h> /* fprintf */
#include <unistd.h> /* getopt */

#include "real_address.h"
#include "create_socket.h"
#include "read_write_loop.h"
#include "wait_for_client.h"

int main(int argc, char *argv[])
{
	int client = 0;
	int port = 12345;
	int opt;
	char *host = "::1";

	while ((opt = getopt(argc, argv, "scp:h:")) != -1) {
		switch (opt) {
			case 's':
				client = 0;
				break;
			case 'c':
				client = 1;
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 'h':
				host = optarg;
				break;
			default:
				fprintf(stderr, "Usage:\n"
								"-s      Act as server\n"
								"-c      Act as client\n"
								"-p PORT UDP port to connect to (client)\n"
								"        or to listen on (server)\n"
								"-h HOST UDP of the server (client)\n"
								"        or on which we listen (server)\n");
				break;
		}
	}
	/* Resolve the hostname */
	struct sockaddr_in6 addr;
	const char *err = real_address(host, &addr);
	if (err) {
		fprintf(stderr, "Could not resolve hostname %s: %s\n", host, err);
		return EXIT_FAILURE;
	}
	/* Get a socket */
	int sfd;
	if (client) {
		sfd = create_socket(NULL, -1, &addr, port); /* Connected */
	} else {
		sfd = create_socket(&addr, port, NULL, -1); /* Bound */
		if (sfd > 0 && wait_for_client(sfd) < 0) { /* Connected */
			fprintf(stderr,
					"Could not connect the socket after the first message.\n");
			close(sfd);
			return EXIT_FAILURE;
		}
	}
	if (sfd < 0) {
		fprintf(stderr, "Failed to create the socket!\n");
		return EXIT_FAILURE;
	}
	/* Process I/O */
	read_write_loop(sfd);

	close(sfd);

	return EXIT_SUCCESS;
}

/* Creates a socket and initialize it
 * @source_addr: if !NULL, the source address that should be bound to this socket
 * @src_port: if >0, the port on which the socket is listening
 * @dest_addr: if !NULL, the destination address to which the socket should send data
 * @dst_port: if >0, the destination port to which the socket should be connected
 * @return: a file descriptor number representing the socket,
 *         or -1 in case of error (explanation will be printed on stderr)
 */
int create_socket(struct sockaddr_in6 *source_addr,
                 int src_port,
                 struct sockaddr_in6 *dest_addr,
                 int dst_port) {
		int sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
		if(sockfd < 0) {
			perror("Socket create failed");
			return -1;
		}
		int client = 1; // true
		if(source_addr == NULL) {
				if(dest_addr == NULL) {
					fprintf(stderr, "source_addr and dest_addr are NULL\n");
					return -1;
				} else {
					if(dst_port <= 0) {
						fprintf(stderr, "Destination port not correctly set\n");
						return -1;
					}
					client = 1;
				}
		} else {
				if(dest_addr == NULL) {
					if(src_port <= 0) {
						fprintf(stderr, "Source port not correctly set\n");
						return -1;
					}
					client = 0;
				} else {
					fprintf(stderr, "source_addr and dest_addr are both non NULL\n");
					return -1;
				}
		}
		if (client) {
			dest_addr->sin6_port = dst_port;
			if(connect(sockfd, (struct sockaddr *) dest_addr, sizeof(struct sockaddr_in6)) == -1) {
				perror("Connecting socked failed");
				close(sockfd);
				return -1;
			}
		} else {
			source_addr->sin6_port = src_port;
			if(bind(sockfd, (struct sockaddr *) source_addr, sizeof(struct sockaddr_in6)) == -1) {
				perror("Binding didn't worked");
				close(sockfd);
				return -1;
			}
		}

		return sockfd;
}

/* Block the caller until a message is received on sfd,
 * and connect the socket to the source addresse of the received message
 * @sfd: a file descriptor to a bound socket but not yet connected
 * @return: 0 in case of success, -1 otherwise
 * @POST: This call is idempotent, it does not 'consume' the data of the message,
 * and could be repeated several times blocking only at the first call.
 */
int wait_for_client(int sfd) {
	#define BUF_SIZE 50
	char buf[BUF_SIZE];
	struct sockaddr_storage fromAddr;
	socklen_t fromAddrLen = sizeof(fromAddr);
	ssize_t bytes_recv = recvfrom(sfd, buf, BUF_SIZE, 0,
																(struct sockaddr*) &fromAddr, &fromAddrLen);
	if (bytes_recv < 0) {
		fprintf(stderr, "No bytes received\n");
		return -1;
	} else {
		if(connect(sfd, (struct sockaddr *) fromAddr, fromAddrLen) == -1) {
			fprintf(stderr, "Try to connect received socket failed\n");
			return -1;
		}
		return 0;
	}
}
