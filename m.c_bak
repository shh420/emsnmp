/*
 *  Embedded SNMP Agent Main Function and Parsing Functions
 *
 *  ./software/ch8/emsnmp/emsnmp.c
 *
 *  M. Tim Jones <mtj@cogitollc.com>
 *
 */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/un.h>
#include <unistd.h>
#include "emsnmp.h"
#include "snmpdata.h"
#include "user.h"
struct messageStruct {
  unsigned char buffer[1025];
  int  len;
  int  index;
};
static struct messageStruct request, response;
unsigned char errorStatus, errorIndex;
typedef struct {
  int start;	/* Absolute Index of the TLV */
  int len;	/* The L value of the TLV */
  int vstart;   /* Absolute Index of this TLV's Value */
  int nstart;   /* Absolute Index of the next TLV */
} tlvStructType;
static int parseLength(const unsigned char *msg, int *len)
{
  int i=1;
  if (msg[0] & 0x80) {
    int tlen = (msg[0] & 0x7f) - 1;
    *len = msg[i++];
    while (tlen--) {
      *len <<= 8;
      *len |= msg[i++];
    }
  } else {
    *len = msg[0];
  }
  return i;
}
static int parseTLV(const unsigned char *msg, int index, tlvStructType *tlv)
{
  int Llen = 0;
  tlv->start = index;
  Llen = parseLength((const char *)&msg[index+1], &tlv->len );
  tlv->vstart = index + Llen + 1;
  switch (msg[index]) {
    case SEQUENCE:
    case GET_REQUEST:
    case GET_NEXT_REQUEST:
    case SET_REQUEST:
      tlv->nstart = tlv->vstart;
      break;
    default:
      tlv->nstart = tlv->vstart + tlv->len;
      break;
  }
  return 0;
}
#define COPY_SEGMENT(x) 
{ 
request.index += seglen; 
    	 memcpy(  &response.buffer[response.index], 
         &request.buffer[x.start], seglen ); 
        response.index += seglen; 
}
/* 
 * Given a TLV and a size, this inserts the size argument into the L element
 * of the response TLV.
 */
static void insertRespLen(int reqStart, int respStart, int size)
{
  int indexStart, lenLength;
  unsigned int mask = 0xff;
  int shift = 0;
  if (request.buffer[reqStart+1] & 0x80) {
    lenLength = request.buffer[reqStart+1] & 0x7f;
    indexStart = respStart+2;
    while (lenLength--) {
      response.buffer[indexStart+lenLength] = 
            (unsigned char)((size & mask) >> shift);
      shift+=8;
      mask <<= shift;
    }
  } else {
    response.buffer[respStart+1] = (unsigned char)(size & 0xff);
  }
}
static int parseVarBind ( int reqType, int index )
{
  int ret = -1, seglen = 0, id;
  tlvStructType name, value;
  int size = 0;
  extern const int maxData;
  ret = parseTLV(request.buffer, request.index, &name);
  if ( request.buffer[name.start] != OBJECT_IDENTIFIER ) return -1;
  id = findEntry(&request.buffer[name.vstart], name.len);
  /* For normal GET_REQUEST/SET_REQUEST, we simply copy the NAME tlv over
   * and continue.  But for GET_NEXT_REQUEST, we need to identify the
   * next OID and then copy it in as if it was the requested object.
   */
  if ((reqType == GET_REQUEST) || (reqType == SET_REQUEST)) {
    seglen = name.nstart - name.start;
    COPY_SEGMENT(name);
    size = seglen;
  } else if (reqType == GET_NEXT_REQUEST) {
    response.buffer[response.index] = request.buffer[name.start];
    if (++id >= maxData) {
      id = OID_NOT_FOUND;
      seglen = name.nstart - name.start;
      COPY_SEGMENT(name);
      size = seglen;
    } else {
      /* Skip the name TLV */
      request.index += name.nstart - name.start;
      getOID(id, &response.buffer[response.index+2], 
              &response.buffer[response.index+1]);
  
      seglen = response.buffer[response.index+1]+2;
      response.index += seglen ;
      size = seglen;
    }
  }
  /* Parse the value TLV, but then replace with ours if we have it */
  ret = parseTLV(request.buffer, request.index, &value);
  if (id != OID_NOT_FOUND) {
    unsigned char dataType;
    int len;
    if ((reqType == GET_REQUEST) || (reqType == GET_NEXT_REQUEST)) {
      getEntry(id, &dataType, &response.buffer[response.index+2], &len);
      response.buffer[response.index] = dataType;
      response.buffer[response.index+1] = len;
      seglen = (2 + len);
      response.index += seglen;
      /* Skip the NULL TLV in the request stream */
      request.index += (value.nstart - value.start);
    } else if (reqType == SET_REQUEST) {
      setEntry(id, &request.buffer[value.vstart], value.len, 
                   request.buffer[value.start], index);
      seglen = value.nstart - value.start;
      COPY_SEGMENT(value);
    }
  } else {
    seglen = value.nstart - value.start;
    COPY_SEGMENT(value);
    errorIndex = index;
    errorStatus = NO_SUCH_NAME;
  }
  size += seglen;
  return size;
}
static int parseSequence ( int reqType, int index )
{
  int ret = -1, seglen;
  tlvStructType seq;
  int size = 0, respLoc;
  ret = parseTLV(request.buffer, request.index, &seq);
  if ( request.buffer[seq.start] != SEQUENCE ) return -1;
  seglen = seq.vstart - seq.start;
  respLoc = response.index;
  COPY_SEGMENT(seq);
  size = parseVarBind( reqType, index );
  insertRespLen(seq.start, respLoc, size);
  size += seglen;
  return size;
}
static int parseSequenceOf ( int reqType )
{
  int ret = -1, seglen;
  tlvStructType seqof;
  int size = 0, respLoc;
  int index = 0;
  ret = parseTLV(request.buffer, request.index, &seqof);
  if ( request.buffer[seqof.start] != SEQUENCE_OF ) return -1;
  seglen = seqof.vstart - seqof.start;
  respLoc = response.index;
  COPY_SEGMENT(seqof);
  while (request.index < request.len) {
    size += parseSequence( reqType, index++ );
  }
  insertRespLen(seqof.start, respLoc, size);
  return size;
}
static int parseRequest ( )
{
  int ret = -1, seglen;
  tlvStructType snmpreq, requestid, errStatus, errIndex;
  int size = 0, respLoc, reqType;
  ret = parseTLV(request.buffer, request.index, &snmpreq);
  reqType = request.buffer[snmpreq.start];
  if ( !VALID_REQUEST(reqType) ) return -1;
  seglen = snmpreq.vstart - snmpreq.start;
  respLoc = snmpreq.start;
  size += seglen;
  COPY_SEGMENT(snmpreq);
  response.buffer[snmpreq.start] = GET_RESPONSE;
  parseTLV(request.buffer, request.index, &requestid);
  seglen = requestid.nstart - requestid.start;
  size += seglen;
  COPY_SEGMENT(requestid);
  parseTLV(request.buffer, request.index, &errStatus);
  seglen = errStatus.nstart - errStatus.start;
  size += seglen;
  COPY_SEGMENT(errStatus);
  parseTLV(request.buffer, request.index, &errIndex);
  seglen = errIndex.nstart - errIndex.start;
  size += seglen;
  COPY_SEGMENT(errIndex);
  ret = parseSequenceOf(reqType);
  if (ret == -1) return -1;
  else size += ret;
  insertRespLen(snmpreq.start, respLoc, size);
  /* Store the error status and index, in the event an error was found */
  if (errorStatus) {
    response.buffer[errStatus.vstart] = errorStatus;
    response.buffer[errIndex.vstart] = errorIndex + 1;
  }
  
  return size;
}
static int parseCommunity ( )
{
  int ret = -1, seglen;
  tlvStructType community;
  int size=0;
  ret = parseTLV(request.buffer, request.index, &community);
  if (!((request.buffer[community.start] == OCTET_STRING) && 
        (community.len == COMMUNITY_SIZE))) return -1;
  if (!bcmp(&request.buffer[community.vstart], 
             (char *)COMMUNITY, COMMUNITY_SIZE)) {
    seglen = community.nstart - community.start;
    size += seglen;
    COPY_SEGMENT(community);
    size += parseRequest();
  } else {
    return -1;
  }
  return size;
}
static int parseVersion ( )
{
  int size = 0, seglen;
  tlvStructType tlv;
  size = parseTLV(request.buffer, request.index, &tlv);
  if (!((request.buffer[tlv.start] == INTEGER) && 
        (request.buffer[tlv.vstart] == SNMP_V1))) return -1;
  seglen = tlv.nstart - tlv.start;
  size += seglen;
  COPY_SEGMENT(tlv);
  size = parseCommunity();
  if (size == -1) return size;
  else return (size + seglen);
}
static int parseSNMPMessage ( )
{
  int size = 0, seglen, ret, respLoc;
  tlvStructType tlv;
  ret = parseTLV(request.buffer, request.index, &tlv);
  if (request.buffer[tlv.start] != SEQUENCE_OF) return -1;
  seglen = tlv.vstart - tlv.start;
  respLoc = tlv.start;
  COPY_SEGMENT(tlv);
  size = parseVersion();
  if (size == -1) return -1;
  else size += seglen;
  insertRespLen(tlv.start, respLoc, size);
  return ret;
}
/*---------------------------------------------------------------------------
 * main() - The embedded SNMP server main
 *-------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  int snmpfd;
  struct sockaddr_in servaddr;
  struct sockaddr from;
  int fromlen;
  int retStatus;
  extern void initTable( void );
  initTable();
  snmpfd = socket(AF_INET, SOCK_DGRAM, 0);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(161);
  retStatus = bind(snmpfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
  if (retStatus < 0) {
    printf("Unable to bind to socket (%d).n", errno);
    exit(-1);
  }
  printf("Started the SNMP agentn");
  for ( ; ; ) {
    fromlen = sizeof(from);
    request.len = recvfrom(snmpfd, 
                            &request.buffer[0], 1024, 0, &from, &fromlen);
    printf("Received datagram from %s, port %dn", 
           inet_ntoa(((struct sockaddr_in *)&from)->sin_addr),
           ((struct sockaddr_in *)&from)->sin_port);
    if (request.len >= 0) {
#if 1
      {
        int i;
        printf("nRequest: n");
        for (i = 0 ; i < request.len ; i++) {
          if ((i % 16) == 0) printf("n  %04x : ", i);
          printf("%02x ", (unsigned char)request.buffer[i]);
        }
        printf("nn");
      }
#endif
      request.index = 0;
      response.index = 0;
      errorStatus = errorIndex = 0;
      if (parseSNMPMessage() != -1) {
        sendto(snmpfd, response.buffer, response.index, 0, 
                (struct sockaddr *)&from, fromlen);
      }
#if 1
      {
        int i;
        printf("nResponse: n");
        for (i = 0 ; i < response.index ; i++) {
          if ((i % 16) == 0) printf("n  %04x : ", i);
          printf("%02x ", (unsigned char)response.buffer[i]);
        }
        printf("nn");
      }
#endif
    }
  }
  close(snmpfd);
  return(0);
}
/*
 *  Copyright (c) 2002 Charles River Media.  All rights reserved.
 * 
 *  Redistribution and use in source and binary forms, with or 
 *  without modification, is hereby granted without fee provided 
 *  that the following conditions are met:
 * 
 *    1.  Redistributions of source code must retain the above 
 *        copyright notice, this list of conditions and the 
 *        following disclaimer.
 *    2.  Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the 
 *        following disclaimer in the documentation and/or other 
 *        materials provided with the distribution.
 *    3.  Neither the name of Charles River Media nor the names of 
 *        its contributors may be used to endorse or promote products 
 *        derived from this software without specific prior 
 *        written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY CHARLES RIVER MEDIA AND CONTRIBUTERS 
 * 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CHARLES
 * RIVER MEDIA OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARAY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */