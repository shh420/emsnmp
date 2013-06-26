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
#include <net/if.h>
#include <sys/ioctl.h>
#include "emsnmp.h"
#include "snmpdata.h"
#include "user.h"
extern dataEntryType snmpData[];
extern const int maxData;
extern time_t startTime;
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

int getlocalip(unsigned char *localipaddr)
{
	int sockfd;
	char ipaddr[50];
	struct sockaddr_in *sin;
	struct ifreq ifr_ip;
	int temp;

	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	memset(&ifr_ip,0,sizeof(ifr_ip));
	strncpy(ifr_ip.ifr_name,"eth0",sizeof(ifr_ip.ifr_name)-1);

	if(ioctl(sockfd,SIOCGIFADDR,&ifr_ip) < 0)
		return "";

	sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;
	strcpy(ipaddr,inet_ntoa(sin->sin_addr));
	printf("sin->sin_addr is %x\n",sin->sin_addr);
//	strncpy(localipaddr,ipaddr,strlen(ipaddr));

	temp = (int)sin->sin_addr.s_addr;

	*localipaddr = (char)temp;
	*(localipaddr+1) = (char)(temp >> 8);
	*(localipaddr+2) = (char)(temp >> 16);
	*(localipaddr+3) = (char)(temp >> 24);

	close(sockfd);
	return 0;
}

int constructversionfield(unsigned char *position,unsigned char value)
{
	*position = 0x02;
	*(position+1) = 0x01;
	*(position+2) = value;

	return 3;
}

int constructcommunityfield(unsigned char *position,unsigned char *value,int len)
{
	*position = 0x04;
	*(position+1) = len;
	memcpy((position+2),value,len);

	return len+2;
}

int constructpdutypefield(unsigned char *position,unsigned char value)
{
	*position = value;

	return 1;
}

int constructenterprisefield(unsigned char *position,unsigned char *value,int len)
{
	*position = 0x06;
	*(position+1) = len;
	memcpy((position+2),value,len);

	return len+2;
}

int constructagentaddrfield(unsigned char *position)
{
	*position = 0x40;
    *(position+1) = 0x04;
	getlocalip(position+2);

	return 6;
}

int constructtraptypefield(unsigned char *position,unsigned char value)
{
	*position = 0x02;
	*(position+1) = 0x01;
	*(position+2) = value;

	return 3;
}

int constructspecificfield(unsigned char *position,int value)
{
	*position = 0x02;
	*(position+1) = 0x04;
	*(position+2) = value;

	return 6;
}

int constructtimestampfield(unsigned char *position)
{
	time_t curtime = time(NULL);

	*position = 0x43;
	*(position+1) = 0x04;
	*(unsigned int *)(position+5) = (unsigned int)(curtime-startTime)*100;
	*(unsigned int *)(position+4) = (unsigned int)(curtime-startTime)*100 >> 8;
	*(unsigned int *)(position+3) = (unsigned int)(curtime-startTime)*100 >> 16;
	*(unsigned int *)(position+2) = (unsigned int)(curtime-startTime)*100 >> 24;
//	*(position+5) = 179656;
//	*(position+4) = 179656 >> 8;
//	*(position+3) = 179656 >> 16;
//	*(position+2) = 179656 >> 24;

	return 6;	
}

int constructtraptlvfield(unsigned char *position,int *ptr,int num)
{
	int len,oidlen,datalen;
	int i,j;
	len = 0;
	for (i=0;i<num;i++)
	{
		oidlen = snmpData[*(ptr+i)].oidlen;
		datalen = snmpData[*(ptr+i)].dataLen;
		*(position+len) = 0x30;
		*(position+1+len) = oidlen + datalen + 4;
		*(position+2+len) = 0x06;
		*(position+3+len) = oidlen;
		memcpy((position+4+len),snmpData[*(ptr+i)].oid,oidlen);
		switch(snmpData[*(ptr+i)].dataType)
		{
			case OCTET_STRING:
				*(position+4+oidlen+len) = 0x04;
				*(position+5+oidlen+len) = datalen;
				memcpy((position+6+oidlen+len),snmpData[*(ptr+i)].u.octetstring,datalen);
				break;
			case OBJECT_IDENTIFIER:
				*(position+4+oidlen+len) = 0x06;
				break;
			case TIME_TICKS:
				*(position+4+oidlen+len) = 0x43;
				break;
			case INTEGER:
				*(position+4+oidlen+len) = 0x02;
				*(position+5+oidlen+len) = datalen;
				memcpy((position+6+oidlen+len),snmpData[*(ptr+i)].u.intval,datalen);
				break;
			default:
				break;
		}
		len += (oidlen + datalen + 6);
	}
	return len;
}

int constructtrappacket(unsigned char *ptr)
{
	unsigned char trapbuf[1024];
	unsigned char tlvbuf[1024];
	unsigned char tempbuf[1024];
	unsigned char *communityfieldvalue;
	unsigned char pdutypevalue;
	int tlvtable[100];
	int totallen;
	int tlvlen;
	int i;
	int temp;
	int templen;

	memset(trapbuf,0,1024);
	memset(tlvbuf,0,1024);
	trapbuf[0] = 0x30;	

	totallen += constructversionfield(&trapbuf[4],0);
		
	communityfieldvalue = "public";
	totallen += constructcommunityfield(&trapbuf[4+totallen],communityfieldvalue,strlen(communityfieldvalue));

//	pdutypevalue = 0xA4;
	totallen += constructpdutypefield(&trapbuf[4+totallen],0xA4);

	//trapbuf[4+totallen] is trapheader and tlv length one byte
	temp = totallen;
	totallen += 1;

	totallen += constructenterprisefield(&trapbuf[4+totallen],snmpData[0].oid,snmpData[0].oidlen);

	totallen += constructagentaddrfield(&trapbuf[4+totallen]);

	totallen += constructtraptypefield(&trapbuf[4+totallen],0x03);

	totallen += constructspecificfield(&trapbuf[4+totallen],0x0);

	totallen += constructtimestampfield(&trapbuf[4+totallen]);

	tlvtable[0] = 3;
	tlvtable[1] = 4;
	tlvtable[2] = 5;
	tlvlen = constructtraptlvfield(tlvbuf,tlvtable,3);
	printf("tlv len is %d\n",tlvlen);
	trapbuf[4+totallen] = 0x30;
	if (tlvlen < 0x80)
	{
		trapbuf[5+totallen] = tlvlen;
		memcpy(&trapbuf[6+totallen],tlvbuf,tlvlen);
	}
	else if (tlvlen <= 0xff)
	{
		trapbuf[5+totallen] = 0x81;
		trapbuf[6+totallen] = tlvlen;
		memcpy(&trapbuf[7+totallen],tlvbuf,tlvlen);
	}
	else
	{
		trapbuf[5+totallen] = 0x82;
		trapbuf[6+totallen] = tlvlen >> 8;
		trapbuf[7+totallen] = tlvlen;
		memcpy(&trapbuf[8+totallen],tlvbuf,tlvlen);
	}
	templen = totallen + tlvlen + 2 - temp - 1;
	if (templen < 0x80)
	{
		trapbuf[4+temp] = templen;
		totallen += (tlvlen + 2);
	}
	else if (templen <= 0xff)
	{
		trapbuf[4+temp] = 0x81;
		memcpy(tempbuf,&trapbuf[5+temp],templen);
		trapbuf[5+temp] = templen;
		memcpy(&trapbuf[6+temp],tempbuf,templen);
		totallen += (tlvlen + 3);
	}
	else
	{
		trapbuf[4+temp] = 0x82;
		memcpy(tempbuf,&trapbuf[5+temp],templen);
		trapbuf[5+temp] = templen >> 8;
		trapbuf[6+temp] = templen;
		memcpy(&trapbuf[7+temp],tempbuf,templen);
		totallen += (tlvlen + 4);
	}

	
	if (totallen < 0x80)
	{
		trapbuf[2] = 0x30;
		trapbuf[3] = totallen;
		memcpy(ptr,&trapbuf[2],totallen+2);
		return totallen+2;
	}
	else if (tlvlen <= 0xff)
	{
		trapbuf[1] = 0x30;
		trapbuf[2] = 0x81;
		trapbuf[3] = totallen;
		memcpy(ptr,&trapbuf[1],totallen+3);
		return totallen+3;
	}
	else
	{
		trapbuf[0] = 0x30;
		trapbuf[1] = 0x82;
		trapbuf[2] = totallen >> 8;
		trapbuf[3] = totallen;
		memcpy(ptr,trapbuf,totallen+4);
		return totallen+4;
	}
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
 *    2.  Redistribuitions in binary form must reproduce the above
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
