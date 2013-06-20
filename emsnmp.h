/*
 *  Embedded SNMP Agent symbolics
 *
 *  ./software/ch8/emsnmp/emsnmp.h
 *  
 *  mtj@cogitollc.com
 *  
 */
#define SNMP_V1	 0
#define COMMUNITY               "public"
#define COMMUNITY_SIZE	 (strlen(COMMUNITY))
#define GET_REQUEST	 0xa0
#define GET_NEXT_REQUEST	0xa1
#define GET_RESPONSE	 0xa2
#define SET_REQUEST	 0xa3
#define VALID_REQUEST(x)	((x == GET_REQUEST)      ||  (x == GET_NEXT_REQUEST) ||(x == SET_REQUEST))
#define INTEGER	 0x02
#define OCTET_STRING	 0x04
#define NULL_ITEM	 0x05
#define OBJECT_IDENTIFIER	0x06
#define SEQUENCE	 0x30
#define SEQUENCE_OF	 SEQUENCE
#define COUNTER	 0x41
#define GAUGE  	 0x42
#define TIME_TICKS	 0x43
#define OPAQUE	 0x44
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
