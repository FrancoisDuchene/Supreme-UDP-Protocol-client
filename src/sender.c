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

  bool failed = false;

  general_status_code prog_status = read_write_loop(sockfd, fd);
  if( prog_status != OK) {
    failed = true;
  }    

  // Ending - On libères les ressources
  close(fd);
  close(sockfd);
  if(failed)
    return EXIT_FAILURE;
  else
    return EXIT_SUCCESS;
}

void printErrorCodeMsg(general_status_code code) {
  switch (code)
  {
  case OK:
    fprintf(stderr, "Tout est OK - fermeture du programme\nCiao\n");
    break;
  case E_NOMEMORY:
    fprintf(stderr, "Pas suffisament de mémoire pour continuer - Echec de la transmission\n");
    break;  
  case E_POLL:
    fprintf(stderr, "Erreur liée à POLL - Echec de la transmission\n");
    break;    
  case E_TIMEOUT:
    fprintf(stderr, "Aucune réaction de chacune des parties - Temps écoulé - Echec de la transmission\n");
    break;
  case E_DECODE:
    fprintf(stderr, "Erreur lié à la procédure de décodage des données - Echec de la transmission\n");
    break;
  case E_ENCODE:
    fprintf(stderr, "Erreur lié à la procédure d'encodage des données - Echec de la transmission\n");
    break;
  case E_SEQNUM_GEN:
    fprintf(stderr, "Erreur lors de la génération de numéro de séquence - Echec de la transmission\n");
    break;
  case E_INCOHERENT:
    fprintf(stderr, "Quelque chose d'incohérent s'est produit dans le programme - Echec de la transmission\n");
    break;
  case E_BUILD:
    fprintf(stderr, "Erreur produite lors de la construction d'un nouveau paquet - Echec de la transmission\n");
    break;
  case E_PKT_QUEUE:
    fprintf(stderr, "Erreur produite dans la gestion de la liste de paquets à retransmettre - Echec de la transmission\n");
    break;
  default:
    fprintf(stderr, "Ce code d'erreur est inconnu au bataillon - Echec de la transmission\n");
    break;
  }
}
