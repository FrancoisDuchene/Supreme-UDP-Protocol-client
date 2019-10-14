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
