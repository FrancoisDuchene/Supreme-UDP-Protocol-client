#include <unistd.h>
#include <stdio.h> /* fprintf */
#include <sys/types.h>
#include <sys/socket.h>

#include "create_socket.h"

/* Creates a socket and initialize it
 * @source_addr: if !NULL, the source address that should be bound to this socket
 * @src_port: if >0, the port on which the socket is listening
 * @dest_addr: if !NULL, the destination address to which the socket should send data
 * @dst_port: if >0, the destination port to which the socket should be connected
 * @return: a file descriptor number representing the socket,
 *         or -1 in case of error (explanation will be printed on stderr)
 */
int create_socket(struct sockaddr_in6 *source_addr,int src_port,struct sockaddr_in6 *dest_addr,int dst_port) {

	int sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	if(sockfd < 0) {
        fprintf(stderr, "Socket create failed\n");
		return -1;
	}

	int client = 0; 
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
		dest_addr->sin6_port = htons(dst_port);
        dest_addr->sin6_addr = in6addr_loopback;
		if(connect(sockfd, (struct sockaddr *) dest_addr, sizeof(struct sockaddr_in6)) == -1) {
            fprintf(stderr, "Connecting socked failed\n");
			close(sockfd);
			return -1;
		}
	} else {
		source_addr->sin6_port = htons(src_port);
      	source_addr->sin6_addr = in6addr_any;
		if(bind(sockfd, (struct sockaddr *) source_addr, sizeof(struct sockaddr_in6)) == -1) {
         	fprintf(stderr, "Binding didn't worked\n");
			close(sockfd);
			return -1;
		}
	}
	return sockfd;
}