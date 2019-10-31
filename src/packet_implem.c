#include "packet_interface.h"

// Auteurs : Duchêne François, Reginster Guillaume
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

/* Extra #includes */
/* Your code will be inserted here */



/* Extra code */
/* Your code will be inserted here */

pkt_t* pkt_new() {
    pkt_t* new_pkt = (pkt_t*) malloc(sizeof(pkt_t));
    if(new_pkt == NULL) { return NULL; }
    new_pkt->type = 0;
    new_pkt->tr = 0;
    new_pkt->window = 0;
    new_pkt->seqnum = 0;
    new_pkt->length = 0;
    new_pkt->timestamp = 0;
    new_pkt->crc1 = 0;
    new_pkt->payload = NULL;
    new_pkt->crc2 = 0;
    return new_pkt;
}

void pkt_del(pkt_t *pkt) {
  if(pkt == NULL){
    return;
  }

  if(pkt_get_length(pkt) != 0 && pkt->payload != NULL){
    free(pkt->payload);
  }
  free(pkt);
}

pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt) {

  pkt_status_code st;
  //Check si header valide
  if(len < 11) {
    return E_NOHEADER;
  }
  //Set type, tr et window
  memcpy(pkt,data,sizeof(uint8_t));

  //Check si le type copié est bon
  if(pkt->type != PTYPE_DATA && pkt->type != PTYPE_ACK && pkt->type != PTYPE_NACK) {
    return E_TYPE;
  }

  //Check si le tr est cohérent
  if(pkt->type != PTYPE_DATA && pkt->tr != 0) {
    return E_TR;
  }

  //Récupération de la longueur du length en byte
  size_t lengthOfLength = varuint_len((uint8_t*) data+1);
  //Set length
  varuint_decode((uint8_t*) data+1,lengthOfLength,&(pkt->length));

  uint16_t size = pkt_get_length(pkt) ;

  //memcpy(&(pkt->length),*(data+1), lengthOfLength);
  if(size > MAX_PAYLOAD_SIZE) {
    return E_LENGTH;
  }
  //Set seqnum
  memcpy(&(pkt->seqnum),data+1+lengthOfLength,sizeof(uint8_t));

  //Set timestamp
  memcpy(&(pkt->timestamp),data+2+lengthOfLength,sizeof(uint32_t));

  //Récupération du crc1 stocké dans le paquet
  memcpy(&(pkt->crc1),data+6+lengthOfLength, sizeof(uint32_t));
  pkt->crc1 = ntohl(pkt->crc1);

  //Calcul du crc1
  uint32_t crc1 = crc32(0L,Z_NULL,0);
  unsigned char *buf = (unsigned char*) malloc(6+lengthOfLength);
  if(buf == NULL) {
    return E_NOMEM;
  }
  memcpy(buf,data,6+lengthOfLength);
  crc1 = crc32(crc1,buf,6+lengthOfLength);

  //Vérification de la validité du crc1
  if(crc1 != pkt->crc1) {
    return E_CRC;
  }
  free(buf);

  //Set payload, check if crc2 is coherent, set crc2
  if(size != 0 && len != (14+lengthOfLength + sizeof(char)*size ) && pkt->tr == 0) {
    return E_UNCONSISTENT;
  }
  else if(size == 0 && len != 10+lengthOfLength && pkt->tr == 0) {	 // signifie pas de payload
    return E_UNCONSISTENT;
  }
  else if(size  == 0 && len == 10+lengthOfLength && pkt->tr == 1) {
    return PKT_OK;
  }

  if(pkt_get_tr(pkt) == 0 && size != 0) { //check shorten package
    st = pkt_set_payload(pkt,data+10+lengthOfLength, size);
    if(st == E_LENGTH || st == E_NOMEM || st == E_LENGTH) {
        return st;
    }

    //Calcul du crc2 en fonction du payload
    uint32_t crc2_calc = crc32(0L,Z_NULL,0);
    unsigned char *buf_crc2 = (unsigned char *) malloc(size);
    if(buf_crc2 == NULL){
      return E_NOMEM;
    }
    memcpy(buf_crc2,data+10+lengthOfLength,size);
    crc2_calc = crc32(crc2_calc,buf_crc2,size);

    //Copie du crc2 stocké dans le paquet
    memcpy(&(pkt->crc2),data+10+lengthOfLength+size,sizeof(uint32_t));
    pkt->crc2 = ntohl(pkt->crc2);

    //Vérification de la validité du crc2
    if(crc2_calc != pkt->crc2)
      return E_CRC;
    free(buf_crc2);
  }
  return PKT_OK;
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len) {
  //copy type, tr & window
  memcpy(buf,pkt,sizeof(uint8_t));
  uint16_t size = pkt_get_length(pkt) ;

  //Récupération longueur du length en byte
  ssize_t lengthOfLength = varuint_predict_len(size);

  //Check validité length
  if(lengthOfLength == -1)
    return E_LENGTH;
  uint8_t *finalLength = (uint8_t*) malloc(lengthOfLength);
  if(finalLength == NULL)
    return E_NOMEM;
  varuint_encode(size, finalLength, lengthOfLength);
  //Copie du length
  memcpy(buf+1,finalLength,sizeof(lengthOfLength));
  free(finalLength);

  //Copie du seqnum
  memcpy(buf+1+lengthOfLength,&(pkt->seqnum),sizeof(uint8_t));

  //Check if package got a payload
  if(pkt_get_tr(pkt) != 0){
    *len=12;
    return PKT_OK;
  }

  //Check if buff is big enough
  if(size > 512) { //512
    return E_LENGTH;
  }

  //Copie du timestamp
  memcpy(buf+2+lengthOfLength,&(pkt->timestamp),sizeof(uint32_t));

  //Calcul du crc1
  uint32_t crc1 = crc32(0L, Z_NULL, 0);
  unsigned char *buf1 = (unsigned char*) malloc(6+lengthOfLength);
  if(buf1 == NULL){
    return E_NOMEM;
  }
  memcpy(buf1,buf,6+lengthOfLength);
  crc1 = crc32(crc1,buf1,6+lengthOfLength);
  uint32_t bufferCRC1 = htonl(crc1);
  //Copie du crc1
  memcpy(buf+6+lengthOfLength, &bufferCRC1, sizeof(uint32_t));
  free(buf1);

  //Check if package got a payload
  if(pkt_get_tr(pkt) != 0){
    *len=12;
    return PKT_OK;
  }
  //Check if buf is big enough
  if(*len > (size_t) size+10+lengthOfLength) {
    return E_UNCONSISTENT;
  }
  //Copie du payload
  memcpy(buf+10+lengthOfLength,pkt->payload,size);

  //Vérification de la présence et validité du payload
  if(pkt_get_payload(pkt) != NULL && pkt_get_tr(pkt) == 0) {

    //Calcul du crc2
    unsigned char *buf2 = (unsigned char *) malloc(size);
    if(buf2==NULL) {
      return E_NOMEM;
    }
    uint32_t crc2 = crc32(0L, Z_NULL, 0);
    memcpy(buf2,pkt->payload,size);
    crc2 = crc32(crc2,buf2,size);
    uint32_t bufferCRC2 = htonl(crc2);
    //Copie du crc2
    memcpy(buf+10+lengthOfLength+size, &bufferCRC2, sizeof(uint32_t));
    free(buf2);

    //Update len
    *len = 14 + lengthOfLength + size;

  }

  return PKT_OK;
}

// GET FUNCTIONS

ptypes_t pkt_get_type  (const pkt_t* pkt) {
    return pkt->type;
}

uint8_t  pkt_get_tr(const pkt_t* pkt)
{
    return pkt->tr;
}

uint8_t  pkt_get_window(const pkt_t* pkt)
{
    return pkt->window;
}

uint8_t  pkt_get_seqnum(const pkt_t* pkt)
{
    return pkt->seqnum;
}

uint16_t pkt_get_length(const pkt_t* pkt) {
  return pkt->length;
}

uint32_t pkt_get_timestamp(const pkt_t* pkt) {
    return pkt->timestamp;
}

uint32_t pkt_get_crc1(const pkt_t* pkt) {
    return pkt->crc1;
}

uint32_t pkt_get_crc2(const pkt_t* pkt) {
  if(pkt_get_payload(pkt) == NULL || pkt_get_tr(pkt) != 0){
    return 0;
  }
  return pkt->crc2;
}

const char* pkt_get_payload(const pkt_t* pkt) {
  if(pkt_get_tr(pkt) != 0){
    return NULL;
  }
  return pkt->payload;
}

// SET FUNCTIONS

pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type) {
  if(type != PTYPE_DATA && type != PTYPE_ACK && type != PTYPE_NACK){
    return E_TYPE;
  }
  pkt->type = type;
  return PKT_OK;
}

pkt_status_code pkt_set_tr(pkt_t *pkt, const uint8_t tr) {
  if(pkt_get_type(pkt) != PTYPE_DATA && tr != 0){
    return E_TR;
  }
  pkt->tr=tr;
  return PKT_OK;
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window) {
  if(window > MAX_WINDOW_SIZE){
    return E_WINDOW;
  }
  pkt->window = window;
  return PKT_OK;
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum) {
  pkt->seqnum = seqnum;
  return PKT_OK;
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length) {
  if(length > MAX_PAYLOAD_SIZE){
    return E_LENGTH;
  }
  pkt->length = length;
  return PKT_OK;
}

pkt_status_code pkt_set_timestamp(pkt_t *pkt, const uint32_t timestamp) {
  pkt->timestamp=timestamp;
  return PKT_OK;
}

pkt_status_code pkt_set_crc1(pkt_t *pkt, const uint32_t crc1) {
  pkt->crc1=crc1;
  return PKT_OK;
}

pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc2) {
  if(pkt_get_tr(pkt) != 0 || pkt_get_payload(pkt) == NULL) {
    return E_CRC;
  }
  pkt->crc2=crc2;
  return PKT_OK;
}

pkt_status_code pkt_set_payload(pkt_t *pkt,
                                const char *data,
                                const uint16_t length) {
  if(length > MAX_PAYLOAD_SIZE){
    return E_LENGTH;
  }
  if(pkt_get_length(pkt) == 0 || data == NULL){
    if(pkt->payload != NULL)
      free(pkt->payload);
    pkt_set_length(pkt,0);
    if(data == NULL) {
      pkt->payload = NULL;
      return PKT_OK;
    }
  }

  pkt->payload=(char *) malloc(sizeof(char )*length);
  if(pkt->payload == NULL){
    return E_NOMEM;
  }
  memcpy(pkt->payload,data,length);
  return pkt_set_length(pkt,length);
}


ssize_t varuint_decode(const uint8_t *data, const size_t len, uint16_t *retval) {
    //Si un seul byte
    if (len == 1) {

      //Copie dans le champs retval donc pkt->length de data avec une longueur de 1 byte
      memcpy(retval,data,1);
      return 1;
    }
    //Si deux bytes
    else if (len == 2) {
      uint16_t data2;
      //Copie dans le champs retval donc pkt->length de data avec une longueur de 2 bytes
      memcpy(&data2,data,2);
      *retval = ntohs(data2); //Host byte-order
      *retval = *retval & 0x7FFF; //Enlève le champs L
      return 2;
    }
    //Si longueur non valide
    else {
      return -1;
    }
}


ssize_t varuint_encode(uint16_t val, uint8_t *data, const size_t len) {
  //longueur en byte valide?
  if(len < 1) {
    return -1;
  }

  ssize_t newLen = (ssize_t) len;
  //Vérifie si le len correspond effectivement à la longueur de val
  if (varuint_predict_len(val) == newLen) {

      //Si plus petit que 8 bits
      if (newLen == 1) {

        //Copie de la valeur
        memcpy(data,&val, 1);
        return 1;
      }
      //si entre 8 et 15 bits
      else if (newLen == 2) {

        //Ajout de 1 pour le champs L
        uint16_t data2 = val | 0x8000;
        data2 = htons(data2);  //Network byte-order
        //Copie de la valeur
        memcpy(data,&data2, 2);
        return 2;
      }
      return -1;
  }
  return -1;
}

size_t varuint_len(const uint8_t *data) {

    //récupération du champs l
    uint8_t l = (*data >> 7) % 2;
    if (l == 0){
      return 1; //1 byte
    }
    return 2; //2 bytes
}


ssize_t varuint_predict_len(uint16_t val) {
    if (val < 128){ //7 bits?
      return 1;
    } else if (val < 32768){ //15 bits?
      return 2;
    }
    return -1; //nombre non valable de bits
}


ssize_t predict_header_length(const pkt_t *pkt){
    if (pkt == NULL ) {
      return -1;
    }

    uint16_t lengthActu = pkt_get_length(pkt); // récupération de la longueur du length
    if (lengthActu < 32768) {
      return varuint_predict_len(lengthActu) + 10; // longueur du length + 10 bytes du header
    } else {    // length non valide
      return -1;
    }
}