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
#include <sys/stat.h>
#include <fcntl.h>

#include <stddef.h> /* size_t */

// Bibliothèques réseaux

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "general.h"

int main (int argc, char **argv);
void printErrorCodeMsg(general_status_code code);

#endif // SENDER_H
