#include "init_connexion.h"

const char * real_address(const char *address, struct sockaddr_in6 *rval) {
	struct addrinfo hints, *res, *addr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
 	hints.ai_socktype = SOCK_DGRAM;
  	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = 0;

	int status = getaddrinfo(address, NULL, &hints, &res);

	if(status != 0 ){
		return gai_strerror(status);
	}

	for (addr = res; addr != NULL; addr = addr->ai_next)
		memcpy(rval, addr->ai_addr, sizeof(struct sockaddr_in6));

	return NULL;
}

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
		if(connect(sockfd, (struct sockaddr *) dest_addr, sizeof(struct sockaddr_in6)) == -1) {
            fprintf(stderr, "Connecting socked failed\n");
			close(sockfd);
			return -1;
		}
	} else {
		source_addr->sin6_port = htons(src_port);
		if(bind(sockfd, (struct sockaddr *) source_addr, sizeof(struct sockaddr_in6)) == -1) {
         	fprintf(stderr, "Binding didn't worked\n");
			close(sockfd);
			return -1;
		}
	}
	return sockfd;
}
