#include <stdlib.h> /* EXIT_X */
#include <stdio.h> /* fprintf */
#include <unistd.h> /* getopt */

#include "real_address.h"
#include "create_socket.h"
#include "read_write_loop.h"
#include "wait_for_client.h"


/* Resolve the resource name to an usable IPv6 address
 * @address: The name to resolve
 * @rval: Where the resulting IPv6 address descriptor should be stored
 * @return: NULL if it succeeded, or a pointer towards
 *          a string describing the error if any.
 *          (const char* means the caller cannot modify or free the return value,
 *           so do not use malloc!)
 */
const char * real_address(const char *address, struct sockaddr_in6 *rval) {
	struct addrinfo hints;
	hints.ai_family = AF_INET6;
 	hints.ai_socktype = SOCK_DGRAM;
  	hints.ai_protocol = IPPROTO_UDP;
	struct addrinfo **res;
	int status = getaddrinfo(address,8000,&hints, res);
	if(status != 0 ){
		return gai_strerror(status);
	}
	memcpy(rval,res,sizeof(struct sockaddr_in6 *));
	free(res);
	return NULL;
}


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
