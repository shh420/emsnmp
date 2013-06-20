/*
 *  Embedded SNMP Agent Data Table Functions
 *
 *  ./software/ch8/emsnmp/snmpdata.c
 *
 *  mtj@cogitollc.com
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "emsnmp.h"
#include "snmpdata.h"
#include "user.h"
extern dataEntryType snmpData[];
extern const int maxData;
/*------------------------------------------------------------------------
 * User callables below:
 *----------------------------------------------------------------------*/
int findEntry ( unsigned char *oid, int len )
{
  int i;
  for (i = 0 ; i < maxData ; i++) {
    if (len == snmpData[i].oidlen) {
      if (!memcmp(snmpData[i].oid, oid, len)) return(i);
    }
  }
  return OID_NOT_FOUND;
}
int getOID ( int id, unsigned char *oid, unsigned char *len)
{
  int j;
  if (!((id >= 0) && (id < maxData))) return INVALID_ENTRY_ID;
  *len = snmpData[id].oidlen;
  for (j = 0 ; j < *len ; j++) {
    oid[j] = snmpData[id].oid[j];
  }
  return SUCCESS;
}
static int getValue( unsigned char *vptr, int vlen)
{
  int index = 0;
  int value = 0;
  while (index < vlen) {
    if (index != 0) value <<= 8;
    value |= vptr[index++];
  }
  return value;
}
int getEntry ( int id, unsigned char *dataType, void *ptr, int *len )
{
  if (!((id >= 0) && (id < maxData))) return INVALID_ENTRY_ID;
  *dataType = snmpData[id].dataType;
  switch(*dataType) {
    case OCTET_STRING :
    case OBJECT_IDENTIFIER :
      {
        unsigned char *string = ptr;
        int j;
        if (snmpData[id].function != NULL) {
          snmpData[id].function( (void *)&snmpData[id].u.octetstring, 
                                  &snmpData[id].dataLen );
        }
        *len = snmpData[id].dataLen;
        for (j = 0 ; j < *len ; j++) {
          string[j] = snmpData[id].u.octetstring[j];
        }
      }
      break;
      
    case INTEGER :
    case TIME_TICKS :
    case COUNTER :
    case GAUGE :
      {
        int *value = ( int * )ptr;
        if (snmpData[id].function != NULL) {
          snmpData[id].function( (void *)&snmpData[id].u.intval,
                                  &snmpData[id].dataLen );
        }
        *len = sizeof(unsigned int);
        *value = htonl(snmpData[id].u.intval);
      }
      break;
    default : 
       return INVALID_DATA_TYPE;
       break;
  }
  return SUCCESS;
}
int setEntry ( int id, void *val, int vlen, unsigned char dataType, int index)
{
  int retStatus=OID_NOT_FOUND;
  int j;
  extern int errorStatus, errorIndex;
  if (snmpData[id].dataType != dataType) {
    errorStatus = BAD_VALUE; 
    errorIndex = index;
    return INVALID_DATA_TYPE;
  }
  switch(snmpData[id].dataType) {
    case OCTET_STRING :
    case OBJECT_IDENTIFIER :
      {
        unsigned char *string = val;
        for (j = 0 ; j < vlen ; j++) {
          snmpData[id].u.octetstring[j] = string[j];
        }
        snmpData[id].dataLen = vlen;
      }
      retStatus = SUCCESS;
      break;
      
    case INTEGER :
    case TIME_TICKS :
    case COUNTER :
    case GAUGE :
      {
        snmpData[id].u.intval = getValue( (unsigned char *)val, vlen);
        snmpData[id].dataLen = vlen;
      }
      retStatus = SUCCESS;
      break;
    default : 
       retStatus = INVALID_DATA_TYPE;
       break;
  }
  return retStatus;
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
