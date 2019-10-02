// Auteurs : Duchêne François, Raquet Damien
#ifndef HEADERS_H
#define HEADERS_H

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

#endif // HEADERS_H
