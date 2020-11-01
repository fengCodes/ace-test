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

/******************************************************************************/
/*                                                                            */
/* Description:                                                               */
/*                                                                            */
/*   This source file implements a sample plugin parser which parses pseudo   */
/*   XML messages. It does not support XML attributes and is intended for     */
/*   illustrative purposes only.                                              */
/*                                                                            */
/* Implementation Notes:                                                      */
/*                                                                            */
/*   Portability:                                                             */
/*                                                                            */
/*     This code is intended to be portable and run on any of the operating   */
/*     systems supported by WebSphere MQ Integrator.                          */
/*                                                                            */
/******************************************************************************/

/* Includes */
/* <malloc.h> is not available on /390 and Mac OS X */
#if !defined(__MVS__) && !defined(__APPLE__)
  #include <malloc.h>
#endif
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BipSampPluginParser.h"
#include <BipCni.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Utility Function: Skip white space from current position     */
/*                                                                            */
/******************************************************************************/
void skipWhiteSpace(
  PARSER_CONTEXT_ST* context
){
  void* p = (char*)context->iBuffer+(int)context->iIndex;

  /* Advance until end of white-space or end of buffer */
  while (CciCharIsSpace(*((CciChar*)p))) {
    context->iIndex++;
    if (context->iIndex == context->iSize) return;
  }

  return;
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Utility Function: Advance to next character                  */
/*                                                                            */
/******************************************************************************/
void advance(
  PARSER_CONTEXT_ST* context,
  CciParser*         parser
){
  int rc = 0;

  /* Advance to the next character */
  context->iIndex++;

  /* Detect and handle the end condition */
  if (context->iIndex == context->iSize) return;

  /* Obtain the next character from the buffer */
  context->iCurrentCharacter = cpiBufferByte(&rc, parser, context->iIndex);
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Utility Function: Advance to end of a valid name             */
/*                                                                            */
/******************************************************************************/
void goToNameEnd(
  PARSER_CONTEXT_ST* context,
  CciParser*         parser
){
  /* Advance until a non-name character is found */
  while (CciCharIsAlnum(context->iCurrentCharacter) ||
         (context->iCurrentCharacter == chUnderscore) ||
         (context->iCurrentCharacter == chColon)) {
    advance(context, parser);
  }
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Utility Function: Advance to end of a tag                    */
/*                                                                            */
/******************************************************************************/
void goToTagEnd(
  PARSER_CONTEXT_ST* context,
  CciParser*         parser
){
  /* Advance until a non-name character is found */
  while (context->iCurrentCharacter != chCloseAngle) {
    advance(context, parser);
  }

  advance(context, parser);
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Utility Function: Parse the next item                        */
/*                                                                            */
/******************************************************************************/
void* parseNextItem(
  CciParser*  parser,
  CciContext* context,
  CciElement* element
){
  void*              endMarker;
  void*              startMarker;
  PARSER_CONTEXT_ST* pc = (PARSER_CONTEXT_ST *)context;
  CciElement*        returnElement = element;
  CciElement*        newElement;
  size_t             markedSize;
  const CciChar*     data;
  int                rc;

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: -> parseNextItem(); parser=0x%p context=0x%p element=0x%p\n",
            parser, context, element);
    fflush(pc->tracefile);
  }

  /* Skip any white space */
  skipWhiteSpace( (PARSER_CONTEXT_ST *)context );

  /* Are we at the end of the buffer? */
  if (pc->iIndex == pc->iSize) {
    if (pc->trace) {
      fprintf(pc->tracefile, "PLUGIN: End of buffer reached...\n");
      fprintf(pc->tracefile, "PLUGIN: <- parseNextItem(); retvalue=0\n");
      fflush(pc->tracefile);
    }
    return(0);
  }

  /* Are we within a tag? */
  if (pc->iInTag) {

    if (pc->iCurrentCharacter == chCloseAngle) {

      /* We have reached the end of a tag */
      pc->iInTag = 0;
      advance( (PARSER_CONTEXT_ST *)context, parser );
    }
    else if (pc->iCurrentCharacter == chForwardSlash) {

      /* We may have reached the end of an empty tag */
      advance( (PARSER_CONTEXT_ST *)context, parser );

      if (pc->iCurrentCharacter == chCloseAngle) {

        pc->iInTag = 0;
        advance( (PARSER_CONTEXT_ST *)context, parser );

        cpiSetElementCompleteNext(&rc, element, 1);
        if (pc->trace) {
          fprintf(pc->tracefile, "PLUGIN: End of tag; setting completeNext to true for element 0x%p\n", element);
          fflush(pc->tracefile);
        }
        returnElement = cpiParent(&rc, element);
      }
      else {
        /* We have a superious "/" character */
      }
    }
    else {

      /* We expect an attribute and the next token to be its name */

      skipWhiteSpace( (PARSER_CONTEXT_ST *)context );

      if (pc->iCurrentCharacter == chEqual) {

        advance( (PARSER_CONTEXT_ST *)context, parser );
        skipWhiteSpace( (PARSER_CONTEXT_ST *)context );

        if (pc->iCurrentCharacter == chDoubleQuote || pc->iCurrentCharacter == chSingleQuote) {

          char quoteChar = (char)pc->iCurrentCharacter;
          advance( (PARSER_CONTEXT_ST *)context, parser );

          /* Get a pointer to the start of the tag */
          startMarker = (char*)pc->iBuffer+(int)pc->iIndex;

          /* Advance to the end of the value */
          while (pc->iCurrentCharacter != quoteChar) {
            advance( (PARSER_CONTEXT_ST *)context, parser );
          }

          /* Get a pointer to the end of the tag */
          endMarker = (char*)pc->iBuffer+(int)pc->iIndex;

          /* Compute the size of the tag */
          markedSize = (size_t)endMarker-(size_t)startMarker;

          /* Convert the attribute value into broker form */
          data = CciNString((char *)startMarker, markedSize, pc->iCcsid);

          /* Create a new name-value element for the attribute */
          newElement = cpiCreateElement(&rc, parser);
          cpiSetElementType(&rc, newElement, CCI_ELEMENT_TYPE_NAME_VALUE);
          cpiSetElementName(&rc, newElement, data);
          if (pc->trace) {
            const char * mbData = mbString(data, pc->iCcsid);
            fprintf(pc->tracefile, "PLUGIN: Created new NAMEVALUE element; object=0x%p type=0x%x name=",
                    newElement, CCI_ELEMENT_TYPE_NAME_VALUE);
            fprintf(pc->tracefile, "%s\n", mbData);
            fflush(pc->tracefile);
            free((void *)mbData);
          }
          /* Free the memory created in CciNString() */
          free((void *)data);

          /* Add the element */
          cpiAddAsLastChild(&rc, element, newElement);

          advance( (PARSER_CONTEXT_ST *)context, parser );
        }
        else {
          /* Syntax error */
        }
      }
      else {
        /* Syntax error */
      }
    }
  }

  /* Find the first tag */
  if (!pc->iInTag) {

    if (pc->iCurrentCharacter == chOpenAngle) {

      /* We appear to have a tag */
      advance( (PARSER_CONTEXT_ST *)context, parser );

      if (pc->iCurrentCharacter == chForwardSlash) {

        /* It is an end tag */
        advance( (PARSER_CONTEXT_ST *)context, parser );

        /* Get a pointer to the start of the tag name */
        startMarker = (char*)pc->iBuffer+(int)pc->iIndex;

        /* Skip over the tag */
        goToNameEnd( (PARSER_CONTEXT_ST *)context, parser );

        /* Get a pointer to the end of the tag */
        endMarker = (char*)pc->iBuffer+(int)pc->iIndex;

        /* Compute the size of the tag */
        markedSize = (size_t)endMarker-(size_t)startMarker;

        cpiSetElementCompleteNext(&rc, element, 1);
        if (pc->trace) {
          fprintf(pc->tracefile, "PLUGIN: End of tag...\n");
          fprintf(pc->tracefile, "PLUGIN: Set completeNext on element; object=0x%p\n",
                  element);
          fflush(pc->tracefile);
        }
        returnElement = cpiParent(&rc, element);

        /* Skip to the end of the tag */
        goToTagEnd( (PARSER_CONTEXT_ST *)context, parser );
      }
      else {

        /* It must be a start tag then */

        /* Get a pointer to the start of the tag */
        startMarker = (char*)pc->iBuffer+(int)pc->iIndex;

        /* Skip over the tag */
        goToNameEnd( (PARSER_CONTEXT_ST *)context, parser );

        /* Get a pointer to the end of the tag */
        endMarker = (char*)pc->iBuffer+(int)pc->iIndex;

        /* Compute the size of the tag */
        markedSize = (size_t)endMarker-(size_t)startMarker;

        /* Convert the tag into broker form */
        data = CciNString((char *)startMarker, markedSize, pc->iCcsid);

        /* Create a new name element for the tag */
        newElement = cpiCreateElement(&rc, parser);
        cpiSetElementType(&rc, newElement, CCI_ELEMENT_TYPE_NAME);
        cpiSetElementName(&rc, newElement, data);
        cpiSetElementCompletePrevious(&rc, newElement, 0);
        cpiSetElementCompleteNext(&rc, newElement, 0);
        if (pc->trace) {
          const char * mbData = mbString(data, pc->iCcsid);
          fprintf(pc->tracefile, "PLUGIN: New tag found\n");
          fprintf(pc->tracefile, "PLUGIN: Created new NAME element; object=0x%p type=0x%x name=",
                  newElement, CCI_ELEMENT_TYPE_NAME);
          fprintf(pc->tracefile, "%s\n", mbData);
          fflush(pc->tracefile);
          free((void *)mbData);
        }
        /* Free the memory allocated in CciNString() */
        free((void *)data);

        /* Add the element */
        cpiAddAsLastChild(&rc, element, newElement);
        cpiSetElementCompletePrevious(&rc, element, 1);
        if (pc->trace) {
          fprintf(pc->tracefile, "PLUGIN: Added 0x%p as lastChild of 0x%p\n",
                  newElement, element);
          fprintf(pc->tracefile, "PLUGIN: Set completePrevious on element; object=0x%p\n", element);
          fflush(pc->tracefile);
        }

        /* Remember we are processing a tag */
        pc->iInTag = 1;

        /* We will be returning the newly created element */
        returnElement = newElement;
      }
    }
    else {

      /* We have some text */

      /* Get a pointer to the start of the text */
      startMarker = (char*)pc->iBuffer+(int)pc->iIndex;

      /* Skip over the text */
      while (pc->iCurrentCharacter != chOpenAngle) {
        advance( (PARSER_CONTEXT_ST *)context, parser );
      }

      /* Get a pointer to the end of the text */
      endMarker = (char*)pc->iBuffer+(int)pc->iIndex;

      /* Compute the size of the text */
      markedSize = (size_t)endMarker-(size_t)startMarker;

      /* Convert the data into broker form */
      data = CciNString((char *)startMarker, markedSize, pc->iCcsid);

      /* Create and set a new value element for the text value */
      newElement = cpiCreateElement(&rc, parser);
      cpiSetElementType(&rc, newElement, CCI_ELEMENT_TYPE_VALUE);
      cpiSetElementCharacterValue(&rc, newElement, data, CciCharLen(data));
      if (pc->trace) {
        /* We don't know the compiler's code page at this stage, but 1208 (utf8 Unicode) */
        /* will generally be appropriate for ASCII systems and 1047 for z/OS USS         */
        #ifdef __MVS__
           const char * mbData = mbString(data, 1047);
        #else
           const char * mbData = mbString(data, 1208);
        #endif

        fprintf(pc->tracefile, "PLUGIN: Text found\n");
        fprintf(pc->tracefile, "PLUGIN: Created new VALUE element; object=0x%p type=0x%x name=",
                newElement, CCI_ELEMENT_TYPE_VALUE);
        fprintf(pc->tracefile, "%s\n", mbData);
        fflush(pc->tracefile);
        free((void *)mbData);
      }
      /* Free the memory allocated in CciNString() */
      free((void *)data);

      cpiAddAsLastChild(&rc, element, newElement);
      if (pc->trace) {
        fprintf(pc->tracefile, "PLUGIN: Added 0x%p as lastChild of 0x%p\n",
                newElement, element);
        fflush(pc->tracefile);
      }
    }
  }

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: <- parseNextItem(); retvalue=0x%p\n",
            returnElement);
    fflush(pc->tracefile);
  }

  return(returnElement);
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Implementation Function:                                     */
/*                                                                            */
/******************************************************************************/
CciContext* cpiCreateContext(
  CciParser* parser
){
  PARSER_CONTEXT_ST *p;

  /* Allocate a pointer to the local context */
  p = (PARSER_CONTEXT_ST *)malloc(sizeof(PARSER_CONTEXT_ST));

  if (p) {
    /* Clear the context area */
    memset(p, 0, sizeof(PARSER_CONTEXT_ST));

    CciCharCpy(p->iNextParserClassName, constPXML);

    /* Initialise default trace setting. Customise this to activate tracing. */
    p->trace = 0;

    /* If tracing, open the file */
    if (p->trace) {
      p->tracefile = fopen("BipSampPluginParser.trc", "w+");
      fprintf(p->tracefile, "PLUGIN: -> cpiCreateContext(); parser=0x%p context=0x%p\n",
              parser, (void*) p);
      fflush(p->tracefile);
    }
  }
  else {
    /* Error: Unable to allocate memory */
  }

  /* Return the pointer to the local context */
  return((CciContext*)p);
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Implementation Function:                                     */
/*                                                                            */
/******************************************************************************/
int cpiParseBufferEncoded(
  CciParser*  parser,
  CciContext* context,
  int         encoding,
  int         ccsid
){
  PARSER_CONTEXT_ST* pc = (PARSER_CONTEXT_ST *)context ;
  int                rc;

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: -> cpiParseBufferEncoded() parser=0x%p context=0x%p encoding=%d ccsid=%d\n",
            parser, context, encoding, ccsid);
    fflush(pc->tracefile);
  }

  /* Get a pointer to the message buffer and set the offset */
  pc->iBuffer = (void *)cpiBufferPointer(&rc, parser);
  pc->iIndex = 0;

  /* Save the format of the buffer */
  pc->iEncoding = encoding;
  pc->iCcsid = ccsid;

  /* Save size of the buffer */
  pc->iSize = cpiBufferSize(&rc, parser);

  /* Prime the first byte in the stream */
  pc->iCurrentCharacter = cpiBufferByte(&rc, parser, pc->iIndex);

  /* Set the current element to the root element */
  pc->iCurrentElement = cpiRootElement(&rc, parser);

  /* Reset flag to ensure parsing is reset correctly */
  pc->iInTag = 0;

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: <- cpiParseBufferEncoded() retvalue=%d\n", pc->iSize);
    fflush(pc->tracefile);
  }

  /* We will assume ownership of the remainder of the buffer */
  return(pc->iSize);
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Implementation Function:                                     */
/*                                                                            */
/******************************************************************************/
void cpiParseFirstChild(
  CciParser*  parser,
  CciContext* context,
  CciElement* element
){
  PARSER_CONTEXT_ST* pc = (PARSER_CONTEXT_ST *)context ;
  int                rc;

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: -> cpiParseFirstChild() parser=0x%p context=0x%p element=0x%p\n",
            parser, context, element);
    fflush(pc->tracefile);
  }

  if ((!cpiElementCompleteNext(&rc, element)) &&
      (cpiElementType(&rc, element) == CCI_ELEMENT_TYPE_NAME)) {

    while ((!cpiElementCompleteNext(&rc, element))     &&
           (!cpiFirstChild(&rc, element)) &&
           (pc->iCurrentElement))
    {
      pc->iCurrentElement = parseNextItem(parser, context, pc->iCurrentElement);
    }
  }

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: <- cpiParseFirstChild()\n");
    fflush(pc->tracefile);
  }

  return;
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Implementation Function:                                     */
/*                                                                            */
/******************************************************************************/
void cpiParseLastChild(
  CciParser*  parser,
  CciContext* context,
  CciElement* element
){
  PARSER_CONTEXT_ST* pc = (PARSER_CONTEXT_ST *)context ;
  int                rc;

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: -> cpiParseLastChild() parser=0x%p context=0x%p element=0x%p\n",
            parser, context, element);
    fflush(pc->tracefile);
  }

  if ((cpiElementType(&rc, element) == CCI_ELEMENT_TYPE_NAME)) {

    while ((!cpiElementCompleteNext(&rc, element))     &&
           (pc->iCurrentElement))
    {
      pc->iCurrentElement = parseNextItem(parser, context, pc->iCurrentElement);
    }
  }

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: <- cpiParseLastChild()\n");
    fflush(pc->tracefile);
  }

  return;
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Implementation Function:                                     */
/*                                                                            */
/******************************************************************************/
void cpiParsePreviousSibling(
  CciParser*  parser,
  CciContext* context,
  CciElement* element
){
  PARSER_CONTEXT_ST* pc = (PARSER_CONTEXT_ST *)context ;

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: -> cpiParsePreviousSibling() parser=0x%p context=0x%p element=0x%p\n",
            parser, context, element);
    fflush(pc->tracefile);
  }

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: <- cpiParsePreviousSibling()\n");
    fflush(pc->tracefile);
  }

  return;
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Implementation Function:                                     */
/*                                                                            */
/******************************************************************************/
void cpiParseNextSibling(
  CciParser*  parser,
  CciContext* context,
  CciElement* element
){
  PARSER_CONTEXT_ST* pc = (PARSER_CONTEXT_ST *)context ;
  int                rc;

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: -> cpiParseNextSibling() parser=0x%p context=0x%p element=0x%p\n",
            parser, context, element);
    fflush(pc->tracefile);
  }

  while ((!cpiElementCompleteNext(&rc, cpiParent(&rc, element))) &&
         (!cpiNextSibling(&rc, element))       &&
         (pc->iCurrentElement))
  {
    pc->iCurrentElement = parseNextItem(parser, context, pc->iCurrentElement);
  }

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: <- cpiParseNextSibling()\n");
    fflush(pc->tracefile);
  }

  return;
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Implementation Function:                                     */
/*                                                                            */
/******************************************************************************/
int cpiWriteBufferEncoded(
  CciParser*  parser,
  CciContext* context,
  int         encoding,
  int         ccsid
){
  PARSER_CONTEXT_ST* pc = (PARSER_CONTEXT_ST *)context ;
  int                initialSize = 0;
  int                rc = 0;

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: -> cpiWriteBufferEncoded() parser=0x%p context=0x%p encoding=%d ccsid=%d\n",
            parser, context, encoding, ccsid);
    fflush(pc->tracefile);
  }

  initialSize = cpiBufferSize(&rc, parser);

  cpiAppendToBuffer(&rc, parser, (CciByte *)"Some test data", 14);

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: <- cpiWriteBufferEncoded()\n");
    fflush(pc->tracefile);
  }

  return cpiBufferSize(0, parser) - initialSize;
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Implementation Function:                                     */
/*                                                                            */
/******************************************************************************/
void cpiDeleteContext(
  CciParser*  parser,
  CciContext* context
){
  PARSER_CONTEXT_ST* pc = (PARSER_CONTEXT_ST *)context ;

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: -> cpiDeleteContext() parser=0x%p context=0x%p\n",
            parser, context);
    fflush(pc->tracefile);
  }

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: <- cpiDeleteContext()\n");
    fflush(pc->tracefile);
  }

  return;
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Implementation Function:                                     */
/*                                                                            */
/******************************************************************************/
void cpiSetElementValue(
  CciParser*       parser,
  CciElement*      element,
  CciElementValue* value
){
  CciElement* newElement;
  int         rc;

  if ((cpiElementType(&rc, element) == CCI_ELEMENT_TYPE_VALUE) ||
      (cpiElementType(&rc, element) == CCI_ELEMENT_TYPE_NAME_VALUE))  {
    cpiSetElementValueValue(&rc, element, value);
  }
  else if (cpiElementType(&rc, element) == CCI_ELEMENT_TYPE_NAME) {
    /* Create a new value element, add as a first child, and set the value */
    newElement = cpiCreateElement(&rc, parser);
    cpiSetElementType(&rc, newElement, CCI_ELEMENT_TYPE_VALUE);
    cpiSetElementValueValue(&rc, newElement, value);
    cpiAddAsFirstChild(&rc, element, newElement);
  }
  else {
  }

  return;
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Implementation Function:                                     */
/*                                                                            */
/******************************************************************************/
const CciElementValue* cpiElementValue(
  CciParser*  parser,
  CciElement* element
){
  CciElement* firstChild;
  const CciElementValue* value = 0;
  int         rc;

  if ((void*)parser == (void*)element) {
    /* Can't access the parser context from here. */
    fprintf(stdout, "PLUGIN: -> cpiElementValue() parser=0x%p element=0x%p\n",
            parser, element);
    fflush(stdout);
  }

  if ((cpiElementType(&rc, element) == CCI_ELEMENT_TYPE_VALUE) ||
      (cpiElementType(&rc, element) == CCI_ELEMENT_TYPE_NAME_VALUE)) {
    value = cpiElementValueValue(&rc, element);
  }
  else if (cpiElementType(&rc, element) == CCI_ELEMENT_TYPE_NAME) {
    firstChild = cniFirstChild(&rc, element);
    value = cpiElementValueValue(&rc, firstChild);
  }
  else {
  }

  return(value);
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Implementation Function:                                     */
/*                                                                            */
/******************************************************************************/
void cpiNextParserClassName(
  CciParser*  parser,
  CciContext* context,
  CciChar*    buffer,
  int         size
){
  PARSER_CONTEXT_ST* pc = (PARSER_CONTEXT_ST *)context ;

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: -> cpiNextParserClassName() parser=0x%p context=0x%p\n",
            parser, context);
    fflush(pc->tracefile);
  }

  /* Copy the name to the broker */
  CciCharNCpy(buffer, pc->iNextParserClassName, size);

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: <- cpiNextParserClassName()\n");
    fflush(pc->tracefile);
  }

  return;
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Implementation Function:                                     */
/*                                                                            */
/******************************************************************************/
void cpiSetNextParserClassName(
  CciParser*  parser,
  CciContext* context,
  CciChar*    name,
  CciBool     isHeaderParser
){
  PARSER_CONTEXT_ST* pc = (PARSER_CONTEXT_ST *)context ;

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: -> cpiSetNextParserClassName() parser=0x%p context=0x%p isHeaderParser=%d\n",
            parser, context, isHeaderParser);
    fflush(pc->tracefile);
  }

  /* Save the name in my context */
  CciCharNCpy(pc->iNextParserClassName, name, CciCharLen(name));

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: <- cpiSetNextParserClassName()\n");
    fflush(pc->tracefile);
  }

  return;
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Implementation Function:                                     */
/*                                                                            */
/******************************************************************************/
int cpiNextParserEncoding(
  CciParser*  parser,
  CciContext* context
){
  PARSER_CONTEXT_ST* pc = (PARSER_CONTEXT_ST *)context ;
  int                encoding = 0;

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: -> cpiNextParserEncoding() parser=0x%p context=0x%p\n",
            parser, context);
    fflush(pc->tracefile);
  }

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: <- cpiNextParserEncoding()\n");
    fflush(pc->tracefile);
  }

  return encoding;
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Implementation Function:                                     */
/*                                                                            */
/******************************************************************************/
int cpiNextParserCodedCharSetId(
  CciParser*  parser,
  CciContext* context
){
  PARSER_CONTEXT_ST* pc = (PARSER_CONTEXT_ST *)context ;
  int                ccsid = 0;

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: -> cpiNextParserCodedCharSetId() parser=0x%p context=0x%p\n",
            parser, context);
    fflush(pc->tracefile);
  }

  if (pc->trace) {
    fprintf(pc->tracefile, "PLUGIN: <- cpiNextParserCodedCharSetId()\n");
    fflush(pc->tracefile);
  }

  return ccsid;
}

/******************************************************************************/
/*                                                                            */
/* Plugin Node Utility function:                                              */
/*                                                                            */
/*  initParserConstants                                                       */
/*                                                                            */
/*  Creates Cci constant strings from defined char * constant strings.        */
/*                                                                            */
/******************************************************************************/
void initParserConstants(
  void
){
  constPXML          = (CciChar*)CciString(CONST_PXML, BIP_DEF_COMP_CCSID);
  constParserFactory = (CciChar*)CciString(CONST_PARSER_FACTORY, BIP_DEF_COMP_CCSID);
}

/******************************************************************************/
/*                                                                            */
/* Plugin Parser Implementation Function:                                     */
/*                                                                            */
/******************************************************************************/
void LilFactoryExportPrefix * LilFactoryExportSuffix bipGetParserFactory(void)
{
  /* Declare variables */
  CciFactory*     factoryObject;
  int             rc;
  static CPI_VFT  vftable = {CPI_VFT_DEFAULT};

  /* Before we proceed we need to initialise all the static constants */
  /* that may be used by the plug-in.                                 */
  initParserConstants();

  /* Setup function table with pointers to parser implementation functions */
  vftable.iFpCreateContext            = cpiCreateContext;
  vftable.iFpParseBufferEncoded       = cpiParseBufferEncoded;
  vftable.iFpParseFirstChild          = cpiParseFirstChild;
  vftable.iFpParseLastChild           = cpiParseLastChild;
  vftable.iFpParsePreviousSibling     = cpiParsePreviousSibling;
  vftable.iFpParseNextSibling         = cpiParseNextSibling;
  vftable.iFpWriteBufferEncoded       = cpiWriteBufferEncoded;
  vftable.iFpDeleteContext            = cpiDeleteContext;
  vftable.iFpSetElementValue          = cpiSetElementValue;
  vftable.iFpElementValue             = cpiElementValue;
  vftable.iFpNextParserClassName      = cpiNextParserClassName;
  vftable.iFpSetNextParserClassName   = cpiSetNextParserClassName;
  vftable.iFpNextParserEncoding       = cpiNextParserEncoding;
  vftable.iFpNextParserCodedCharSetId = cpiNextParserCodedCharSetId;

  /* Create the parser factory for this plugin */
  factoryObject = cpiCreateParserFactory(&rc, constParserFactory);
  if (factoryObject) {
    /* Define the classes of message supported by the factory */
    cpiDefineParserClass(&rc, factoryObject, constPXML, &vftable);
  }
  else {
    /* Error: Unable to create parser factory */
  }

  /* Return address of this factory object to the broker */
  return(factoryObject);
}

#ifdef __cplusplus
}
#endif

