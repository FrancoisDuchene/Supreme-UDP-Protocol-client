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

	while (1){

		if (select(sfd + 1, &readsfd, &writesfd, NULL, &timeout) == -1) {
			printf (" Error select \n");
			return ;
		}

		fread(buf, strlen(buf)+1, 1, STDIN_FILENO);
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
				free (buf);
				printf("End\n");
                break;
			}
			fwrite(buf2, strlen(buf2)+1, 1, STDOUT_FILENO);
		}
		
	}	
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
