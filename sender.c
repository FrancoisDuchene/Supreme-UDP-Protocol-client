#include "headers.h"

#define MAXLINE 1024

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

  // On génère le socket
  int sockfd = init_socket();
  if(sockfd < 0) {
    if(file != NULL)
        close(fd);
    return EXIT_FAILURE;
  }

  // On génère l'adresse
  struct addrinfo *address = get_addr_struct(host, port);
  if(address == NULL) {
    if(file != NULL)
      close(fd);
    close(sockfd);
    return EXIT_FAILURE;
  }
  // On envoie quelque chose de simple

  send_test_message(sockfd, address);

  // On reçoit le message

  recv_test_message(sockfd, address);

  // Ending
  close(fd);
  close(sockfd);
  free(address);
  return EXIT_SUCCESS;
}

int send_test_message(int sockfd, struct addrinfo *server) {
  pkt_t* pkt_to_send = pkt_new();
  char *hello = "ping";
  pkt_set_payload(pkt_to_send,hello, strlen(hello));
  pkt_set_type(pkt_to_send, PTYPE_DATA);
  pkt_set_timestamp(pkt_to_send, 0);
  pkt_set_seqnum(pkt_to_send,0);
  pkt_set_window(pkt_to_send,1);

  size_t *buf_len = (size_t*) malloc(sizeof(size_t));
  memset(buf_len,20,1);
  char buffer[20];
  pkt_status_code st = pkt_encode(pkt_to_send, buffer, buf_len);
  if(st != PKT_OK) {
    pkt_del(pkt_to_send);
    return -1;
  }

  ssize_t bytes_sent = sendto(sockfd, buffer, *buf_len, 0, server->ai_addr,server->ai_addrlen);
  if(bytes_sent < 0) {
    fprintf(stderr, "No bytes has been sent\n");
    pkt_del(pkt_to_send);
    return -1;
  }else if(bytes_sent != (ssize_t) *buf_len) {
    fprintf(stderr, "The amount of bytes sent is not the total length of the message\nBytes sent: %lu\nInitial buffer length: %zu\n", bytes_sent, *buf_len);
    pkt_del(pkt_to_send);
    return -1;
  }
  pkt_del(pkt_to_send);
  return bytes_sent;
}

void recv_test_message(int sockfd, struct addrinfo *server) {
  char buffer[MAXLINE];
  struct sockaddr_storage fromAddr;
  socklen_t fromAddrLen = sizeof(fromAddr);
  ssize_t bytes_recv = recvfrom(sockfd, buffer, MAXLINE, 0,
                        (struct sockaddr*) &fromAddr, &fromAddrLen);
  if (bytes_recv < 0) {
    fprintf(stderr, "No bytes found\n");
    return;
  }else{
    printf("%s\n", "Reçu qqch !");
  }

  server->ai_addr = server->ai_addr;

  pkt_t* pkt_recv = pkt_new();
  pkt_status_code st = pkt_decode(buffer,bytes_recv,pkt_recv);
  if(st != PKT_OK) {
    printf("%s\nCode d'erreur reçu : %u\n", "Aïe, coup dur pour Guillaume", st);
    pkt_del(pkt_recv);
    return;
  }
  printf("%s\n", "coucou");
  // if (!SockAddrsEqual(server->ai_addr, (struct sockaddr *) &fromAddr)) {
  //   fprintf("Received a packet from unknow source\n");
  //   return;
  // }
  print_pkt(pkt_recv, false);

  pkt_del(pkt_recv);
}

/*
 * Create a new socket for UDP/IPv6 and return it's file description.
 * @return: a file description, return < 0 if an error occured
 */
int init_socket() {
  int sockfd = socket(AF_INET6,SOCK_DGRAM,IPPROTO_UDP);
  if(sockfd < 0) {
    perror("socket creation failed");
  }
  return sockfd;
}

/*
 * Get a structure address containing the adress to send to
 * @param host: the server address contained in a char*
 * @param port: the server port contained in a char*
 * @return: an addrinfo structure pointer dynamically allocated, or NULL if an error occured.
 * BEWARE free has to be called on that pointer
 */
struct addrinfo* get_addr_struct(const char *host, const char *port) {
  int status = 0;
  struct addrinfo hints;
  struct addrinfo *servinfo;

  memset(&hints, 0, sizeof(hints));
  //hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;
  status = getaddrinfo(host, port, &hints, &servinfo);
  if(status != 0) {
      fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
      return NULL;
  }

  struct addrinfo *rep = (struct addrinfo*) malloc(sizeof(struct addrinfo));
  if(rep == NULL) {
      fprintf(stderr, "Erreur malloc\n");
      freeaddrinfo(servinfo);
      return NULL;
  }

  memcpy(rep, servinfo, sizeof(struct addrinfo));
  rep->ai_next = NULL;

  freeaddrinfo(servinfo);
  return rep;
}

void print_pkt(const pkt_t* pkt, bool print_payload) {
  bool payload_is_empty = false;
  // On load en cache tous les paramètres
  const ptypes_t type = pkt_get_type(pkt);
  const uint8_t tr = pkt_get_tr(pkt);
  const uint8_t window = pkt_get_window(pkt);
  const uint8_t seqnum = pkt_get_seqnum(pkt);
  const uint16_t length = pkt_get_length(pkt);
  const uint32_t timestamp = pkt_get_timestamp(pkt);
  const uint32_t crc1 = pkt_get_crc1(pkt);
  const char *payload = pkt_get_payload(pkt);
  const uint32_t crc2 = pkt_get_crc2(pkt);
  // On les imprime en fonction de leur contenu
  switch (type) {
    case PTYPE_DATA:
      printf("C'est un paquet DATA !\n");
      break;
    case PTYPE_ACK:
      printf("C'est un paquet ACK !\n");
      break;
    case PTYPE_NACK:
      printf("C'est un paquet NACK !\n");
      break;
    default:
      printf("C'est un harent !\n");
  }
  if(tr == 0) printf("Ce n'est pas un paquet tronqué\n");
  else printf("Planqué-vous, c'est un paquet tronqué !\n");
  printf("La window est actuellement de %u\n", window);
  printf("Son numéro de séquence est de %u\n", seqnum);
  if(length == 0) {
    printf("Le paquet ne contient pas de payload ! (longueur nulle)\n");
    payload_is_empty = true;
  }else{
    printf("La longueur du payload est de %u bytes\n", length);
  }
  printf("Le timestamp est de %u\n", timestamp);
  printf("Le crc1 vaut : %u\n", crc1);
  if(payload_is_empty || !print_payload) {
    printf("Il n'y a pas de payload\n");
  }else{
    printf("Le payload vaut (en supposant qu'il ne soit constitué que de char) :\n");
    for (size_t i = 0; i < length; i++) {
      printf("%c\n", payload[i]);
    }
  }
  printf("\nLe crc2 vaut : %u\n", crc2);
}
