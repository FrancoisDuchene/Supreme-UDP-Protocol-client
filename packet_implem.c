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

struct __attribute__((__packed__)) pkt {
  uint8_t type : 2; // 2bits
  uint8_t tr : 1; //1bit
  uint8_t window : 5; // 5bits
  uint16_t length; // 16bits nbo
  uint8_t seqnum; // 8bits
  uint32_t timestamp; // 32 bits
  uint32_t crc1; //32bits
  char *payload; // max 4096bits
  uint32_t crc2; // 32bits
};

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
  //check there is a header
  if(len < 12) {
    return E_NOHEADER;
  }
  //set type, tr et window
  memcpy(pkt,data,sizeof(uint8_t));

  //Check si le type copier est bon
  if(pkt->type != PTYPE_DATA && pkt->type != PTYPE_ACK && pkt->type != PTYPE_NACK){
    return E_TYPE;
  }
  //Check si le tr est cohérent
  if(pkt->type != PTYPE_DATA && pkt->tr != 0){
    return E_TR;
  }
  //set seqnum
  memcpy(&(pkt->seqnum),data+1,sizeof(uint8_t));

  //set length
  memcpy(&(pkt->length),data+2,sizeof(uint16_t));
  if(pkt_get_length(pkt) > MAX_PAYLOAD_SIZE){
    return E_LENGTH;
  }
    //set timestamp
  memcpy(&(pkt->timestamp),data+4,sizeof(uint32_t));

  //check if crc1 is coherent & set crc1
  memcpy(&(pkt->crc1),data+8, sizeof(uint32_t));
  pkt->crc1 = ntohl(pkt->crc1);

  uint32_t crc1 = crc32(0L,Z_NULL,0);
  unsigned char *buf = (unsigned char*) malloc(8);
  if(buf == NULL) {
    return E_NOMEM;
  }
  memcpy(buf,data,8);
  crc1 = crc32(crc1,buf,8);

  if(crc1 != pkt->crc1) {
        free(buf);
    return E_CRC;
  }
  free(buf);

  //set payload, check if crc2 is coherent, set crc2
  if(pkt_get_length(pkt) != 0 && len != (16 + sizeof(char)*pkt_get_length(pkt)) && pkt->tr == 0) {
    return E_UNCONSISTENT;
  }
  else if(pkt_get_length(pkt) == 0 && len != 12 && pkt->tr ==0) {	 // signifie pas de payload
    return E_UNCONSISTENT;
  }
  else if(pkt_get_length(pkt) == 0 && len == 12 && pkt->tr == 1) {
    return PKT_OK;
  }

  if(pkt_get_tr(pkt) == 0 && pkt_get_length(pkt) != 0) { //check shorten package
    pkt_set_payload(pkt,data+12, pkt_get_length(pkt));
        //if(st == E_LENGTH || st == E_NOMEM || st == E_LENGTH) {
          //  return st;
        //}

    uint16_t size = pkt_get_length(pkt);
    uint32_t crc2_calc = crc32(0L,Z_NULL,0);
    unsigned char *buf_crc2 = (unsigned char *) malloc(size);
    if(buf_crc2 == NULL)
      return E_NOMEM;
    memcpy(buf_crc2,data+12,size);
    crc2_calc = crc32(crc2_calc,buf_crc2,size);
    memcpy(&(pkt->crc2),data+12+size,4);
    pkt->crc2 = ntohl(pkt->crc2);
    if(crc2_calc != pkt->crc2)
      return E_CRC;
    free(buf_crc2);
  }
  return PKT_OK;
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len) {
  //copy type, tr & window
  memcpy(buf,pkt,sizeof(uint8_t));

  //copy seqnum
  memcpy(buf+1,&(pkt->seqnum),sizeof(uint8_t));

  //check if package got a payload
  if(pkt_get_tr(pkt) != 0){
    *len=12;
    return PKT_OK;
  }

  //check if buff is big enough
  if(pkt_get_length(pkt) > 512) { //512
    return E_LENGTH;
  }

  //change to nbo and copy length
  //uint16_t *taille_ptr = (uint16_t *) malloc(sizeof(uint16_t));
    //if(taille_ptr == NULL) {return E_NOMEM;}
  uint16_t taille_ptr = htons(pkt_get_length(pkt));
  memcpy(buf+2,&taille_ptr,sizeof(uint16_t));
  //free(taille_ptr);

  //copy timestamp
  memcpy(buf+4,&(pkt->timestamp),sizeof(uint32_t));

  //copy crc1
  uint32_t crc1 = crc32(0L, Z_NULL, 0);
  unsigned char *buf1=(unsigned char*) malloc(8);
  if(buf1 == NULL){
    return E_NOMEM;
  }
  memcpy(buf1,buf,8);

  crc1 = crc32(crc1,buf1,8);
  uint32_t bufferCRC1=htonl(crc1);
  memcpy(buf+8,&bufferCRC1,sizeof(uint32_t));
  free(buf1);

  //check if package got a payload
  if(pkt_get_tr(pkt) != 0){
    *len=12;
    return PKT_OK;
  }
  //check if buf is big enough
  if(*len < (size_t)pkt_get_length(pkt)+12){
    return E_NOMEM;
  }
  //copy payload
  memcpy(buf+12,pkt->payload,pkt_get_length(pkt));

  // compute crc2, set payload, set crc2
  if(pkt_get_payload(pkt) != NULL && pkt_get_tr(pkt) == 0)
  {

    unsigned char *buf2=(unsigned char *) malloc(pkt_get_length(pkt));
    if(buf2==NULL){
      return E_NOMEM;
    }
    uint32_t crc2=crc32(0L, Z_NULL, 0);

    memcpy(buf2,pkt->payload,pkt_get_length(pkt));

    crc2 = crc32(crc2,buf2,pkt_get_length(pkt));
    crc2 = htonl(crc2);
    memcpy(buf+12+pkt_get_length(pkt),&crc2,sizeof(uint32_t));
    free(buf2);
    //update len
    *len=16+pkt_get_length(pkt);
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
  if (pkt->length == 0) return 0;
  return pkt->length;
}

uint32_t pkt_get_timestamp   (const pkt_t* pkt) {
    return pkt->timestamp;
}

uint32_t pkt_get_crc1   (const pkt_t* pkt)
{
    return pkt->crc1;
}

uint32_t pkt_get_crc2   (const pkt_t* pkt)
{
  if(pkt_get_payload(pkt) == NULL || pkt_get_tr(pkt) != 0){
    return 0;
  }
  return pkt->crc2;
}

const char* pkt_get_payload(const pkt_t* pkt)
{
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

pkt_status_code pkt_set_tr(pkt_t *pkt, const uint8_t tr)
{
  if(pkt_get_type(pkt) != PTYPE_DATA && tr != 0){
    return E_TR;
  }
  pkt->tr=tr;
  return PKT_OK;
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
  if(window > MAX_WINDOW_SIZE){
    return E_WINDOW;
  }
  pkt->window = window;
  return PKT_OK;
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
  pkt->seqnum = seqnum;
  return PKT_OK;
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
  if(length > MAX_PAYLOAD_SIZE){
    return E_LENGTH;
  }
  if(pkt->length != NULL)
    free(pkt->length);
  //TODO
  pkt->length = (uint16_t) malloc(sizeof(uint16_t) - 1);
  pkt->length=htons(length);
  return PKT_OK;
}

pkt_status_code pkt_set_timestamp(pkt_t *pkt, const uint32_t timestamp)
{
  pkt->timestamp=timestamp;
  return PKT_OK;
}

pkt_status_code pkt_set_crc1(pkt_t *pkt, const uint32_t crc1)
{
  pkt->crc1=crc1;
  return PKT_OK;
}

pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc2)
{
  if(pkt_get_tr(pkt) != 0 || pkt_get_payload(pkt) == NULL) {
    return E_CRC;
  }
  pkt->crc2=crc2;
  return PKT_OK;
}

pkt_status_code pkt_set_payload(pkt_t *pkt,
                                const char *data,
                                const uint16_t length)
{
  if(length > MAX_PAYLOAD_SIZE){
    return E_LENGTH;
  }
  if(pkt_get_length(pkt) == 0 || data == NULL){
    if(pkt->payload != NULL)
      free(pkt->payload);
    pkt_set_length(pkt,0);
  }

  pkt->payload=(char *) malloc(sizeof(char )*length);
  if(pkt->payload == NULL){
    return E_NOMEM;
  }
  memcpy(pkt->payload,data,length);
  return pkt_set_length(pkt,length);
}


ssize_t varuint_decode(const uint8_t *data, const size_t len, uint16_t *retval) {

    uint8_t data2;
    data2 = data << 1;
    if (len == 1) {

      retval = data2;

    } else if (len == 2) {

      retval = ntohs(data2);

    } else {
      return -1;
    }
}


ssize_t varuint_encode(uint16_t val, uint8_t *data, const size_t len) {
    if (varuint_predict_len(val)==len) {

      if (len == 1) {

        memcpy(data,val, 1);
        data[0] = 0;  
        
      } else if (len == 2) {

        memcpy(data,val, 2);
        data = htons(data);
        data[0] = 1;

      }

    } else {
      return -1;
    }
}

size_t varuint_len(const uint8_t *data) {
    uint8_t l = data[0];
    if (l == 0){
      return 1;
    }
    return 2;
}


ssize_t varuint_predict_len(uint16_t val)
{
    if (val < 128){
      return 1;
    } else if (val < 32768){
      return 2;
    } 

    return -1;
}


ssize_t predict_header_length(const pkt_t *pkt){
    if (pkt == NULL ) {
      return -1;
    }

    uint16_t lengthActu = pkt_get_length(pkt);
    if (lengthActu < 32768) {
      return varuint_predict_len(lengthActu) + 10;
    } else {
      return -1;
    }
}

int main() {
  printf("%s\n", "Coucou");

}
