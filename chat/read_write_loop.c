#include <stdlib.h> /* EXIT_X */
#include <stdio.h> /* fprintf */
#include <sys/select.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include "read_write_loop.h"

/* Loop reading a socket and printing to stdout,
 * while reading stdin and writing to the socket
 * @sfd: The socket file descriptor. It is both bound and connected.
 * @return: as soon as stdin signals EOF
 */
void read_write_loop(int sfd){

	fd_set readsfd;
	struct timeval timeout;
	char buf[1024];
    char buf2[1024];

	timeout.tv_sec = 3;
	timeout.tv_usec = 500000;
    
    FD_ZERO(&readsfd);
    FD_SET(STDIN_FILENO, &readsfd);
	FD_SET(sfd, &readsfd);

	while (1) { 
       
		if (select(sfd + 1, &readsfd, NULL, NULL, &timeout) == -1) {
			printf (" Error select \n");
			return ;
		}
        
        if(FD_ISSET(STDIN_FILENO, &readsfd)){
            ssize_t readL = read(STDIN_FILENO,buf,sizeof(buf));
            
			if(readL == EOF){
				break;
			}
            
            size_t readLen = (size_t)readL;

			if((int) write(sfd,buf,readLen) == -1){
                printf(" Error write1 \n");
				return;
            }
		}
		
        if(FD_ISSET(sfd, &readsfd)){
            size_t readLen = (size_t)read(sfd,buf2,sizeof(buf2));

			if((int) write(STDOUT_FILENO,buf2,readLen) == -1){
                printf(" Error write2 \n");
				return;
            }
		}    
    }
}