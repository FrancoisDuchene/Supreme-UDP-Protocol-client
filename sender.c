#include "sender.h"
#include "general.h"
#include "init_connexion.h"
#include "packet_interface.h"
#include "read_write_loop_final.h"

#define MAXLINE 1024

int main (int argc, char **argv) {
  int opt;
  char *file = NULL;
  if (argc < 2) {
      fprintf(stderr, "`%s' arguments missing\n", argv[0]);
      fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
      return EXIT_FAILURE;
  }
  static struct option options[] =
  {
    {"filename", required_argument, 0, 'f'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}
  };

  while ((opt = getopt_long(argc, argv, "f:h:", options, NULL)) != -1) {
      switch (opt) {
          case 'f':
              file = optarg;
              break;
          case 'h':
              printf("Usage: %s hostname port\n", argv[0]);
              printf("       %s [-f X] hostname port\n", argv[0]);
              printf("       %s [--filename X] hostname port\n", argv[0]);
              printf("\n");
              return EXIT_FAILURE;
          default:
              fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
              return EXIT_FAILURE;
      }
  }

  const char *host = argv[argc-2];
  const int port = atoi(argv[argc-1]);
  if(port < 1024) {
    fprintf(stderr, "Port invalide");
    return EXIT_FAILURE;
  }

  int fd;
  if(file == NULL) {
    fd = STDIN_FILENO;
  } else {
    fd = open(file,O_RDONLY);
    if(fd == -1) {
      fprintf(stderr, "Impossible d'ouvrir le fichier - nom incorrect\n");
      return EXIT_FAILURE;
    }
  }

  // On génère l'adresse
  struct sockaddr_in6 addr;
  char *err = real_address(host, &addr);
  if(err != NULL) {
    fprintf(stderr, "Error: %s", err);
    close(fd);
    return EXIT_FAILURE;
  }

  // On génère le socket et on le connecte
  int sockfd = create_socket(NULL, -1, &addr, port);

  // Boucle du sender

  read_write_loop(sockfd, fd);

  // Ending - On libères les ressources
  close(fd);
  close(sockfd);
  return EXIT_SUCCESS;
}
