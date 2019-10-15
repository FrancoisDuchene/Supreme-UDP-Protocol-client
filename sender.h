#ifndef SENDER_H
#define SENDER_H

// Bibliothèques standards

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <stddef.h> /* size_t */
#include <stdint.h> /* uintx_t */

// Bibliothèques réseaux

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

// Bibliothèques où il faut inclure une extension dans le MakeFile

#include <zlib.h>

int main (int argc, char **argv);
int send_test_message(int sockfd, struct addrinfo *server);
void recv_test_message(int sockfd, struct addrinfo *server);
int init_socket();
struct addrinfo* get_addr_struct(const char *host, const char *port);
void print_pkt_osef(pkt_t* pkt, bool print_payload);

#endif // SENDER_H
