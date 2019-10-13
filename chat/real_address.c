#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h> /* memset */

#include "real_address.h"

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
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
 	hints.ai_socktype = SOCK_DGRAM;
  	hints.ai_protocol = IPPROTO_UDP;
	struct addrinfo *res;
	char *port = "8000";
	int status = getaddrinfo(address, port, &hints, &res);
	if(status != 0 ){
		return gai_strerror(status);
	}
	memcpy(rval, res, sizeof(struct sockaddr_in6));
	free(res);
	return NULL;
}
