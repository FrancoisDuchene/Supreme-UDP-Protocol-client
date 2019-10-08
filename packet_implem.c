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
  uint8_t window : 5; // 5bits
  uint8_t tr : 1; //1bit
  uint8_t type : 2; // 2bits
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
  
  pkt_status_code st;
  //check there is a header
  if(len < 11) {
    return E_NOHEADER;
  }
  //set type, tr et window
  memcpy(pkt,data,sizeof(uint8_t));

  //Check si le type copié est bon
  if(pkt->type != PTYPE_DATA && pkt->type != PTYPE_ACK && pkt->type != PTYPE_NACK) {
    return E_TYPE;
  }
  //Check si le tr est cohérent
  if(pkt->type != PTYPE_DATA && pkt->tr != 0) {
    return E_TR;
  }
  size_t lengthOfLength = varuint_len((uint8_t*) data+1);
  //set length
  varuint_decode((uint8_t*) data+1,lengthOfLength,&(pkt->length));

  uint16_t size = pkt_get_length(pkt) ;

  //memcpy(&(pkt->length),*(data+1), lengthOfLength);
  if(size > MAX_PAYLOAD_SIZE) {
    return E_LENGTH;
  }
  //set seqnum
  memcpy(&(pkt->seqnum),data+1+lengthOfLength,sizeof(uint8_t));

    //set timestamp
  memcpy(&(pkt->timestamp),data+2+lengthOfLength,sizeof(uint32_t));

  //check if crc1 is coherent & set crc1
  memcpy(&(pkt->crc1),data+6+lengthOfLength, sizeof(uint32_t));
  pkt->crc1 = ntohl(pkt->crc1);

  uint32_t crc1 = crc32(0L,Z_NULL,0);
  unsigned char *buf = (unsigned char*) malloc(6+lengthOfLength);
  if(buf == NULL) {
    return E_NOMEM;
  }
  memcpy(buf,data,6+lengthOfLength);
  crc1 = crc32(crc1,buf,6+lengthOfLength);

  if(crc1 != pkt->crc1) {
    return E_CRC;
  }
  free(buf);

  //set payload, check if crc2 is coherent, set crc2
 
  if(size== 0 && len != (14+lengthOfLength + sizeof(char)*size ) && pkt->tr == 0) {
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

    uint32_t crc2_calc = crc32(0L,Z_NULL,0);
    unsigned char *buf_crc2 = (unsigned char *) malloc(size);
    if(buf_crc2 == NULL){
      return E_NOMEM;
    }
    memcpy(buf_crc2,data+10+lengthOfLength,size);
    crc2_calc = crc32(crc2_calc,buf_crc2,size);

    memcpy(&(pkt->crc2),data+10+lengthOfLength+size,sizeof(uint32_t));
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

  uint16_t size = pkt_get_length(pkt) ;

  //change to nbo and copy length

  ssize_t lengthOfLength = varuint_predict_len(size);

  if(lengthOfLength == -1)
    return E_LENGTH;
  uint8_t *finalLength = (uint8_t*) malloc(lengthOfLength);
  if(finalLength == NULL)
    return E_NOMEM;
  varuint_encode(size, finalLength, lengthOfLength);
  memcpy(buf+1,finalLength,sizeof(lengthOfLength));
  free(finalLength);

  //copy seqnum
  memcpy(buf+1+lengthOfLength,&(pkt->seqnum),sizeof(uint8_t));

  //check if package got a payload
  if(pkt_get_tr(pkt) != 0){
    *len=12;
    return PKT_OK;
  }

  //check if buff is big enough
  if(size > 512) { //512
    return E_LENGTH;
  }

  //copy timestamp
  memcpy(buf+2+lengthOfLength,&(pkt->timestamp),sizeof(uint32_t));

  //copy crc1
  uint32_t crc1 = crc32(0L, Z_NULL, 0);
  unsigned char *buf1 = (unsigned char*) malloc(6+lengthOfLength);
  if(buf1 == NULL){
    return E_NOMEM;
  }
  memcpy(buf1,buf,6+lengthOfLength);

  crc1 = crc32(crc1,buf1,6+lengthOfLength);
  uint32_t bufferCRC1 = htonl(crc1);
  memcpy(buf+6+lengthOfLength, &bufferCRC1, sizeof(uint32_t));
  free(buf1);

  //check if package got a payload
  if(pkt_get_tr(pkt) != 0){
    *len=12;
    return PKT_OK;
  }
  //check if buf is big enough
  if(*len < (size_t) size+12) {
    return E_NOMEM;
  }
  //copy payload
  memcpy(buf+10+lengthOfLength,pkt->payload,size);

  // compute crc2, set payload, set crc2
  if(pkt_get_payload(pkt) != NULL && pkt_get_tr(pkt) == 0) {

    unsigned char *buf2 = (unsigned char *) malloc(size);
    if(buf2==NULL) {
      return E_NOMEM;
    }
    uint32_t crc2 = crc32(0L, Z_NULL, 0);

    memcpy(buf2,pkt->payload,size);

    crc2 = crc32(crc2,buf2,size);
    uint32_t bufferCRC2 = htonl(crc2);
    memcpy(buf+10+lengthOfLength+size, &bufferCRC2, sizeof(uint32_t));
    free(buf2);
    //update len
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
    uint16_t data2;
    if (len == 1) {
      data2 = *data;
      *retval = data2;

      return 1;
    } else if (len == 2) {
      data2 = *data;
      *retval = ntohs(data2);
      *retval = *retval & 0x7FFF;

      return 2;
    } else {
      return -1;
    }
}


ssize_t varuint_encode(uint16_t val, uint8_t *data, const size_t len) {
  if(len < 1) {
    return -1;
  }
  ssize_t newlen = (ssize_t) len;
    if (varuint_predict_len(val) == newlen) {

      if (newlen == 1) {

        memcpy(data,&val, 1);

        return 1;
      } else if (newlen == 2) {
        uint16_t data2 = val | 0x8000;
        data2 = htons(data2);

        memcpy(data,&data2, 2);

        return 2;
      }
      return -1;
    }
    return -1;
}

size_t varuint_len(const uint8_t *data) {
    uint8_t l = (*data >> 7) % 2;
    //printf("l: %u\n", l);
    if (l == 0){
      return 1;
    }
    return 2;
}


ssize_t varuint_predict_len(uint16_t val) {
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
/*
int main() {
  pkt_t *yo = pkt_new();
  pkt_set_type(yo, PTYPE_DATA);
  pkt_set_tr(yo, 0);
  pkt_set_window(yo, 2);
  pkt_set_length(yo, 24);
  pkt_set_seqnum(yo, 230);
  pkt_set_timestamp(yo, 1010);
  char *buf = (char*) malloc(20);
  size_t ahah = 20;
  pkt_encode(yo,buf,&ahah);
  pkt_t *aie = pkt_new();
  pkt_decode(buf,20,aie);
  printf("type : %u\n", pkt_get_type(aie));
  printf("tr : %u\n", pkt_get_tr(aie));
  printf("window : %u\n", pkt_get_window(aie));
  printf("length : %u\n", pkt_get_length(aie));
  printf("seqnum : %u\n", pkt_get_seqnum(aie));
  printf("timestamp : %u\n", pkt_get_timestamp(aie));
  printf("crc1 : %u\n", pkt_get_crc1(aie));
  pkt_del(yo);
}
*/
