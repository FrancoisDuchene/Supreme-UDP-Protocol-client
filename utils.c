void print_pkt_osef(pkt_t* pkt, bool print_payload) {
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
