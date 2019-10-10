#ifndef SENDER_H
#define SENDER_H

#include "headers.h"

int main (int argc, char **argv);
int send_test_message(int sockfd, struct addrinfo *server);
void recv_test_message(int sockfd, struct addrinfo *server);
int init_socket();
struct addrinfo* get_addr_struct(const char *host, const char *port);
void print_pkt(const pkt_t* pkt, bool print_payload);

#endif // SENDER_H
