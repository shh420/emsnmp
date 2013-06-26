/* 
 *  Embedded SNMP Agent User Function Prototypes and Symbolics 
 *
 *  ./software/ch8/emsnmp/user.h
 *
 *  mtj@cogitollc.com
 *
 */
/* Prototypes */
int findEntry ( unsigned char *, int );
int getEntry ( int, unsigned char *, void *, int * );
int insertEntry ( unsigned char *, int, unsigned char, 
                   void *, int, void (*func)(void *, unsigned char *) );
int setEntry ( int, void *, int, unsigned char, int );
int getOID ( int, unsigned char *, unsigned char * );
int getlocalip(unsigned char *localipaddr);

int constructversionfield(unsigned char *position,unsigned char value);
int constructcommunityfield(unsigned char *position,unsigned char *value,int len);
int constructpdutypefield(unsigned char *position,unsigned char value);
int constructenterprisefield(unsigned char *position,unsigned char *value,int len);
int constructagentaddrfield(unsigned char *position);
int constructtraptypefield(unsigned char *position,unsigned char value);
int constructspecificfield(unsigned char *position,int value);
int constructtimestampfield(unsigned char *position);
int constructtraptlvfield(unsigned char *position,int *ptr,int num);
int constructtrappacket(unsigned char *ptr);
/* SNMP User function return codes */
#define SUCCESS	 0
#define OID_NOT_FOUND	 -1
#define TABLE_FULL	 -2
#define ILLEGAL_LENGTH	 -3
#define INVALID_ENTRY_ID	-4
#define INVALID_DATA_TYPE	-5
/* SNMP Message Error Codes */
#define NO_SUCH_NAME	 2
#define BAD_VALUE   	 3
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
