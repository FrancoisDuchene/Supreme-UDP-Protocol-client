#include "headers.h"

int main (int argc, char **argv) {
  int opt;
  char *port;
  char *host;
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

  host = argv[argc-2];
  port = argv[argc-1];

  int fd;
  if(file == NULL) {
    fd = STDIN_FILENO;
  } else {
    fd = open(file,O_RDONLY);
    if(fd == -1) {
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}
