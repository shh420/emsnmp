/*
 *  Embedded SNMP user table and dynamic functions
 *
 *  ./software/ch8/emsnmp/user.c
 *
 *  mtj@cogitollc.com
 *
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "emsnmp.h"
#include "snmpdata.h"
#include "user.h"
time_t startTime;
static void currentUptime(void *, unsigned char *);
static void syncStatus(void *, unsigned char *);
static void signalStrength(void *, unsigned char *);
dataEntryType snmpData[]={
  /* System MIB */
    /* SysDescr Entry */
    {8, {0x2b, 6, 1, 2, 1, 1, 1, 0}, 
       OCTET_STRING, 17, {"Embedded Computer"}, 
       NULL},
  
    /* SysObjectID Entry */
    {8, {0x2b, 6, 1, 2, 1, 1, 2, 0}, 
       OBJECT_IDENTIFIER, 8, {"x2bx06x01x02x01x01x02x00"},
       NULL},
    /* SysUptime Entry */
    {8, {0x2b, 6, 1, 2, 1, 1, 3, 0}, 
       TIME_TICKS, 0, {""},
       currentUptime},
    /* sysContact Entry */
    {8, {0x2b, 6, 1, 2, 1, 1, 4, 0}, 
       OCTET_STRING, 15, {"mtj@mtjones.com"}, 
       NULL},
    /* sysName Entry */
    {8, {0x2b, 6, 1, 2, 1, 1, 5, 0}, 
       OCTET_STRING, 14, {"ec.mtjones.com"}, 
       NULL},
    /* Location Entry */
    {8, {0x2b, 6, 1, 2, 1, 1, 6, 0}, 
       OCTET_STRING, 5, {"B3B44"},
       NULL},
    /* SysServices */
    {8, {0x2b, 6, 1, 2, 1, 1, 7, 0}, 
       INTEGER, 4, {""}, 
       NULL},
  /* Experimental MIB for the Antenna manager */
    /* Antenna Azimuth Commanded Position */
    {6, {0x2b, 6, 1, 3, 1, 0},
      INTEGER, 4, {""},
      NULL},
    /* Antenna Elevation Commanded Position */
    {6, {0x2b, 6, 1, 3, 2, 0},
      INTEGER, 4, {""},
      NULL},
    /* Antenna Receiver Sync Status */
    {6, {0x2b, 6, 1, 3, 3, 0},
      OCTET_STRING, 0, {""},
      syncStatus},
    /* Antenna Receiver Signal Strength Status */
    {6, {0x2b, 6, 1, 3, 4, 0},
      INTEGER, 4, {""},
      signalStrength}
};
const int maxData = (sizeof(snmpData) / sizeof(dataEntryType));
/* MIB Initialization */
void initTable( void )
{
  /* Items must be in OID order */
  startTime = time(NULL);
  /* Note:  The C spec permits the auto-initialization of the first member
   * of a union.  Therefore, since the first member of the union is a string,
   * we initialize the 'u.intval' elements here.
   */
  snmpData[6].u.intval = 5; /* System.Services */
  snmpData[7].u.intval = 0; /* Azimuth   */
  snmpData[8].u.intval = 0; /* Elevation */
}
/* Dynamic Data Functions */
static void currentUptime(void *ptr, unsigned char *len)
{
  time_t curTime = time(NULL);
  *(unsigned int *)ptr = (unsigned int)(curTime - startTime) * 100;
  *len = 4;
}
static int sigStr = 0;
static void syncStatus(void *ptr, unsigned char *len)
{
  if (snmpData[8].u.intval > 90) {
    *len = sprintf((char *)ptr, "detected");
    sigStr = 99;
  } else {
    *len = sprintf((char *)ptr, "no signal");
    sigStr = 0;
  }
}
static void signalStrength(void *ptr, unsigned char *len)
{
  *(unsigned int *)ptr = sigStr;
  *len = 4;
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
