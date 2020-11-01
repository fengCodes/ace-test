/*
 * Sample program for use with Product         
 *  ProgIds: 5724-J06 5724-J05 5724-J04 5697-J09 5655-M74 5655-M75 5648-C63
 *  (C) Copyright IBM Corporation 1999, 2000.
 * All Rights Reserved * Licensed Materials - Property of IBM
 *
 * This sample program is provided AS IS and may be used, executed,
 * copied and modified without royalty payment by customer
 *
 * (a) for its own instruction and study,
 * (b) in order to develop applications designed to run with an IBM
 *     WebSphere product, either for customer's own internal use or for
 *     redistribution by customer, as part of such an application, in
 *     customer's own products.
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BIPSAMPPLUGINUTIL_H
#define BIPSAMPPLUGINUTIL_H

/* Define a suitable value for the default code page for the compiler which */
/* is used to compile the plugin. 1208 (UTF-8 UNICODE) will generally be    */
/* appropriate for UNIX and NT platforms, and 1047 for OS/390 and z/OS      */
#ifdef __MVS__
#define BIP_DEF_COMP_CCSID 1047
#else
#define BIP_DEF_COMP_CCSID 1208
#endif

#include <BipCci.h>

/* ---------------------------------------------------------------- */
/*                     Internal constants                           */
/*                                                                  */
/* The following are the characters that are of interest to us      */
/* when parsing XML.                                                */
/*                                                                  */
/* ---------------------------------------------------------------- */
#define chTab           '\t'
#define chCR            '\r'
#define chSpace         ' ' 
#define chDoubleQuote   '\"'
#define chSingleQuote   '\''
#define chForwardSlash  '/'
#define chColon         ':'
#define chOpenAngle     '<'
#define chEqual         '='
#define chCloseAngle    '>'
#define chBackSlash     '\\'
#define chUnderscore    '_'

/* 0-9 is contiguous in ASCII and EBCDIC */
#define chDigit_0       '0'
#define chDigit_9       '9'

/* A-Z is contiguous in ASCII but not in EBCDIC */
#define chLatin_A       'A'
#define chLatin_Z       'Z'

/* a-z is contiguous in ASCII but not in EBCDIC */
#define chLatin_a       'a'
#define chLatin_z       'z'

#ifdef __MVS__
#define chLatin_I       'I'
#define chLatin_J       'J'
#define chLatin_R       'R'
#define chLatin_S       'S' 
#define chLatin_i       'i'
#define chLatin_j       'j'
#define chLatin_r       'r'
#define chLatin_s       's'
#endif

/* Function prototypes */
int CciCharIsSpace(
  const CciChar c
);

int CciCharIsAlnum(
  const CciChar c
);

void CciCharCpy(
  CciChar*       target,
  const CciChar* source
);

void CciCharNCpy(
  CciChar*       target,
  const CciChar* source,
  int            size
);

int CciCharLen(
  const CciChar* s
);

int CciCharCmp(
  const CciChar* s1,
  const CciChar* s2
);

const CciChar* CciString(
  const char* source,
  int         codepage
);

const CciChar* CciNString(
  const char*        source,
  const size_t       size,
  int                codepage
);

const char* mbString(
  const CciChar* source,
  int            codepage
);

void checkRC(
  int inRC
);

#endif

#ifdef __cplusplus
}
#endif

