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

/* Includes */
/* <malloc.h> is not available on /390 and Mac OS X */
#if !defined(__MVS__) && !defined(__APPLE__)
  #include <malloc.h>
#endif
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BipSampPluginUtil.h"

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************/
/* Plug-in node utility functions                                                             */
/*   CciCharIsSpace : Returns 0 if CciChar is not space, otherwise returns 1                  */
/*   CciCharIsAlnum : Returns 0 if CciChar is not alpha-numberic, otherwise returns 1         */
/*   CciCharCpy     : Copy CciChar strings, like strcpy                                       */
/*   CciCharNcpy    : Copy CciChar strings, like strncpy                                      */
/*   CciCharCmp     : Compare CciChar strings, like strcmp                                    */
/*   CciCharLen     : Find length of CciString, like strlen, result in characters (not bytes) */
/*   CciString      : Convert an ordinary string to Unicode, result must be free()'d          */
/*                    after use, intended for use with source-code constants                  */
/*   CciNString     : Convert N bytes of a string to Unicode, result must be free()'d         */
/*                    after use, intended for use with source-code constants                  */
/*   mbString       : Convert Unicode to an ordinary string, result must be free()'d          */
/*                    after use, intended for use to produce human-readable text              */
/*                                                                                            */
/**********************************************************************************************/
int CciCharIsSpace(
  const CciChar c
){
#ifdef __MVS__
  if (c == chSpace)
#else
  if (((c >= chTab) && (c <= chCR)) || (c == chSpace))
#endif
  {
     return 1;
  }
  else
  {
     return 0;
  }
}

int CciCharIsAlnum(
  const CciChar c
){
#ifdef __MVS__
  if (((c >= chDigit_0) && (c <= chDigit_9)) ||
      ((c >= chLatin_A) && (c <= chLatin_I)) ||
      ((c >= chLatin_J) && (c <= chLatin_R)) ||
      ((c >= chLatin_S) && (c <= chLatin_Z)) ||
      ((c >= chLatin_a) && (c <= chLatin_i)) ||
      ((c >= chLatin_j) && (c <= chLatin_r)) ||
      ((c >= chLatin_s) && (c <= chLatin_z)))
#else
  if (((c >= chDigit_0) && (c <= chDigit_9)) ||
      ((c >= chLatin_A) && (c <= chLatin_Z)) ||
      ((c >= chLatin_a) && (c <= chLatin_z)))
#endif
  {
     return 1;
  }
  else
  {
     return 0;
  }
}

void CciCharCpy(
  CciChar*       target,
  const CciChar* source
){
  CciChar c ;
  for (;;)
    {
      c = *source ;
      source += 1 ;
      *target = c ;
      target += 1 ;
      if ( 0 == c ) break ;
    }
}

void CciCharNCpy(
  CciChar*       target,
  const CciChar* source,
  int            size)
{
  CciChar c ;
  int i;
  for (i=0;i<size;i+=1)
    {
      c = *source ;
      *target = c ;
      target += 1 ;
      if ( 0 != c ) source += 1 ;
    }
}

int CciCharLen(
  const CciChar* s
){
  int i=0 ;
  while (*s != 0 )
    {
      s += 1 ;
      i += 1 ;
    }
  return i ;
}

int CciCharCmp(
  const CciChar* s1,
  const CciChar* s2
){
  CciChar c1 ;
  CciChar c2 ;
  for (;;)
    {
      c1 = *s1 ;
      s1 += 1 ;
      c2 = *s2 ;
      s2 += 1 ;
      if ( (0 == c1) || (c1 != c2) ) break ;
    }
  return c1-c2 ;
}

const CciChar* CciString(
  const char* source,
  int         codepage
){
  /* Maximum number of characters in Unicode representation */
  size_t maxChars = strlen(source) + 1 ;
  CciChar* buffer = (CciChar*)malloc(maxChars * sizeof(CciChar)) ;
  int rc ;
  cciMbsToUcs(&rc, source, buffer, (int)(maxChars & 0x7FFFFFFF), codepage) ;
  return buffer ;
}

const CciChar* CciNString(
  const char*        source,
  const size_t       size,
  int                codepage)
{
  /* Maximum number of characters in Unicode representation */
  /* The bytes may not be NULL terminated so we need to make a copy */
  size_t bufferSize = ((size + 1) * sizeof(CciChar));
  int rc;
  CciChar* buffer = (CciChar*)malloc(bufferSize);
  if (buffer) {
    memset(buffer, '\0', bufferSize);
    memcpy(buffer, source, size);
  }
  cciMbsToUcs(&rc, source, buffer, (int)(size & 0x7FFFFFFF), codepage) ;
  return buffer ;
}

const char* mbString(
  const CciChar* source,
  int            codepage
){
  int charCount = CciCharLen(source) ;
  int maxBytes = (charCount+1) * 9 ;
  char * buffer = (char *)malloc((size_t)(maxBytes & 0x7FFFFFFF)) ;
  int rc ;
  cciUcsToMbs(&rc, source, buffer, maxBytes, codepage) ;
  return buffer ;
}

void checkRC(
  int rcIn)
{
  /* if an exception occurred re-throw it and let the broker handle it               */
  /* any other return code must be handled by the caller as they are specific to the */
  /* particular utility function that was called                                     */
  
  if (rcIn != CCI_SUCCESS) {
    if (rcIn == CCI_EXCEPTION) { 
      cciRethrowLastException(NULL);
    }
    else {
        /* place any generic error handling code here to be executed in the case where */
        /* the utility function did not return success but there was no exception      */
    }
  }
}

#ifdef __cplusplus
}
#endif
