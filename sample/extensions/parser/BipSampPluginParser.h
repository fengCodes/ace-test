
#ifndef BIPSAMPPLUGINPARSER_H
#define BIPSAMPPLUGINPARSER_H

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

#include <BipCpi.h>
#include <BipSampPluginUtil.h>

typedef struct parserContext {
  void*   iBuffer;                     /* Buffer pointer */
  int     iEncoding;                   /* Encoding of buffer */
  int     iCcsid;                      /* CCSID of buffer */
  int     iSize;                       /* Size of input buffer in bytes */
  int     iIndex;                      /* Current position in the buffer */
  CciChar iCurrentCharacter;           /* The current character */
  void*   iCurrentElement;             /* Current element */
  int     iInTag;                      /* Flag: Currently parsing a tag? */
  int     trace;                       /* Flag: Trace active? */
  FILE*   tracefile;                   /* Trace file stream */
  CciChar iNextParserClassName[250];   /* The name of the next parser in the chain */
} PARSER_CONTEXT_ST;

/* ---------------------------------------------------------------- */
/*                     Internal constants                           */
/*                                                                  */
/* The interface expects some CciChar strings to be passed. These   */
/* have to be constructed from char strings using the supplied      */
/* utilities. The char constants are defined here along with the    */
/* static CciChar variables which will contain these constants      */
/* converted into CciChar format.                                   */
/*                                                                  */
/* ---------------------------------------------------------------- */
#define CONST_PXML                    "PXML"
CciChar * constPXML                   = 0;

#define CONST_PARSER_FACTORY          "ComIbmSampleParserFactory"
CciChar * constParserFactory          = 0;

#ifdef __cplusplus
}
#endif



#endif  /* BIPSAMPPLUGINPARSER_H*/

