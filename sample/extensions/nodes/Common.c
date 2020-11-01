/* Includes */
/* <malloc.h> is not available on /390 and Mac OS X */
#if !defined(__MVS__) && !defined(__APPLE__)
  #include <malloc.h>
#endif
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Common.h"
#include <BipCni.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                                                                            */
/* Plugin Node Implementation Function:                     cniGetAttribute() */
/*                                                                            */
/******************************************************************************/
int _getAttribute(
  CciContext* context,
  CciChar*    attrName,
  CciChar*    buffer,
  int         bufsize
){
  static const char* functionName = "_getAttribute()";
  ATTRIBUTE_TBL_ENTRY* p = ((NODE_CONTEXT_ST *)context)->attrListHead;
  NODE_CONTEXT_ST*     nc = (NODE_CONTEXT_ST *)context;
  char                 string[256];

  if (nc->trace) {
    const char *mbNodeName = mbString(nc->nodeName, BIP_DEF_COMP_CCSID);
    const char *mbAttrName = mbString(attrName, BIP_DEF_COMP_CCSID);
    fprintf(nc->tracefile, "NODE: <%s> -> %s context=0x%p attrName='%s'\n",
            mbNodeName, functionName, context, mbAttrName);
    fflush(nc->tracefile);
    free((void *)mbNodeName);
    free((void *)mbAttrName);
  }

  while (p != 0) {
    if (CciCharCmp(p->name, attrName) == 0) {

      switch(p->type) {
      case CNI_TYPE_INTEGER:
        #ifndef _MSC_VER
          snprintf((char*)&string, 256, "%d", *((int*)p->valptr));
        #else
          _snprintf_s((char*)&string, 256, _TRUNCATE, "%d", *((int*)p->valptr));
        #endif
        {
          const CciChar* ucsAttrValue = CciString(string, BIP_DEF_COMP_CCSID); /*TODO*/
          CciCharNCpy(buffer, ucsAttrValue, min(bufsize, 256));
          free((void *)ucsAttrValue);
        }
        break;
      case CNI_TYPE_STRING:
        {
          const CciChar* ucsAttrValue = CciString((char *)p->valptr, BIP_DEF_COMP_CCSID); /* TODO */
          CciCharNCpy(buffer, ucsAttrValue, min(MAX_STRING_SIZE, bufsize));
          free((void *)ucsAttrValue);
        }
      default:
        /* Error: Invalid datatype */
        break;
      }

      if (nc->trace) {
        const char *mbNodeName = mbString(nc->nodeName, BIP_DEF_COMP_CCSID);
        fprintf(nc->tracefile, "NODE: <%s> <- %s rc=0\n",
                mbNodeName, functionName);
        fflush(nc->tracefile);
        free((void *)mbNodeName);
      }

      /* Normal return; attribute value returned */
      return(0);
    }
    p = (ATTRIBUTE_TBL_ENTRY *)p->next;
  }

  if (nc->trace) {
    const char *mbNodeName = mbString(nc->nodeName, BIP_DEF_COMP_CCSID);
    fprintf(nc->tracefile, "NODE: <%s> <- %s rc=1\n",
            mbNodeName, functionName);
    fflush(nc->tracefile);
    free((void *)mbNodeName);
  }

  /* Attribute not owned by plugin */
  return(1);
}

/******************************************************************************/
/*                                                                            */
/* Plugin Node Implementation Function:                cniDeleteNodeContext() */
/*                                                                            */
/* This function is called when an instance of a node is deleted.             */
/*                                                                            */
/******************************************************************************/
void _deleteNodeContext(
  CciContext* context
){
  static const char* functionName = "_deleteNodeContext()";
  NODE_CONTEXT_ST* nc = (NODE_CONTEXT_ST *)context;
  if (nc->trace) {
    const char *mbNodeName = mbString(nc->nodeName, BIP_DEF_COMP_CCSID);
    fprintf(nc->tracefile, "NODE: <%s> -> %s context=0x%p\n",
            mbNodeName, functionName, context);
    fflush(nc->tracefile);
    free((void *)mbNodeName);
  }
  return;
}



/******************************************************************************/
/*                                                                            */
/* Plugin Node Implementation Function:                     cniGetAttribute() */
/*                                                                            */
/******************************************************************************/
int _getAttributeName(
  CciContext* context,
  int         index,
  CciChar*    buffer,
  int         bufsize
){
  static const char* functionName = "_getAttributeName()";
  NODE_CONTEXT_ST* nc = (NODE_CONTEXT_ST *)context;
  int                  count = 0;
  ATTRIBUTE_TBL_ENTRY *p = ((NODE_CONTEXT_ST *)context)->attrListHead;

  if (nc->trace) {
    const char *mbNodeName = mbString(nc->nodeName, BIP_DEF_COMP_CCSID);
    fprintf(nc->tracefile, "NODE: <%s> -> %s context=0x%p index=%d buffer=0x%p bufsize=%d\n",
            mbNodeName, functionName, context, index, (void*) buffer, bufsize);
    fflush(nc->tracefile);
    free((void *)mbNodeName);
  }

  while (p != 0) {
    if (count == index) {
      CciCharCpy(buffer, p->name);

      /* Normal return; attribute name returned */
      return(0);
    }
    count++;
    p = (ATTRIBUTE_TBL_ENTRY *)p->next;
  }

  /* Attribute not owned by the plugin node; defer to base */
  return(1);
}

/******************************************************************************/
/*                                                                            */
/* Plugin Node Implementation Function:                     cniSetAttribute() */
/*                                                                            */
/******************************************************************************/
int _setAttribute(
  CciContext* context,
  CciChar*    attrName,
  CciChar*    attrValue
){
  static const char* functionName = "_setAttribute()";
  ATTRIBUTE_TBL_ENTRY* p = ((NODE_CONTEXT_ST *)context)->attrListHead;
  NODE_CONTEXT_ST*     nc = (NODE_CONTEXT_ST *)context;

  if (nc->trace) {
    const char *mbNodeName = mbString(nc->nodeName, BIP_DEF_COMP_CCSID);
    const char *mbAttrName = mbString(attrName, BIP_DEF_COMP_CCSID);
    const char *mbAttrValue = mbString(attrValue, BIP_DEF_COMP_CCSID);
    fprintf(nc->tracefile,
      "NODE: <%s> -> %s context=0x%p attrName='%s' attrValue='%s'\n",
      mbNodeName, functionName, context, mbAttrName, mbAttrValue);
    fflush(nc->tracefile);
    free((void *)mbNodeName);
    free((void *)mbAttrName);
    free((void *)mbAttrValue);
  }

  while (p != 0) {
    if (CciCharCmp(p->name, attrName) == 0) {

      switch(p->type) {
      case CNI_TYPE_INTEGER:
        {
          const char* mbAttrValue = mbString(attrValue, BIP_DEF_COMP_CCSID);
          *((int *)p->valptr) = atoi(mbAttrValue);
          free((void *)mbAttrValue);
        }

        break;
      case CNI_TYPE_STRING:
        {
          const char* mbAttrValue = mbString(attrValue, BIP_DEF_COMP_CCSID);
          size_t len = strlen(mbAttrValue);
          if(len > MAX_STRING_SIZE)
            len = MAX_STRING_SIZE;
          memset(p->valptr, 0, MAX_STRING_SIZE);
          memcpy(p->valptr, mbAttrValue, len);
          free((void *)mbAttrValue);
        }
        break;

      default:
        /* Error: Invalid datatype */
        break;
      }

      if (nc->trace) {
        const char *mbNodeName = mbString(nc->nodeName, BIP_DEF_COMP_CCSID);
        fprintf( nc->tracefile, "NODE: <%s> <- %s rc=0\n",
                 mbNodeName, functionName);
        fflush(nc->tracefile);
        free((void *)mbNodeName);
      }

      /* Normal return; attribute value stored by plugin */
      return(0);
    }
    p = (ATTRIBUTE_TBL_ENTRY *)p->next;
  }

  if (nc->trace) {
    const char* mbNodeName = mbString(nc->nodeName, BIP_DEF_COMP_CCSID);
    fprintf(nc->tracefile, "NODE: <%s> <- %s rc=1\n",
            mbNodeName, functionName);
    fflush(nc->tracefile);
    free((void *)mbNodeName);
  }

  /* Attribute not owned by plugin */
  return(1);
}


/******************************************************************************/
/*                                                                            */
/* Plugin Node Utility function:                                              */
/*                                                                            */
/*  Traverse the syntax element tree from a given point                       */
/*                                                                            */
/******************************************************************************/
void Traverse(
  CciElement* element
){
  static int depth = -1;
  CciElement* firstChild = 0;
  CciElement* nextSibling = 0;
  int         rc = 0;
  CciChar     elementName[256];

  if (element != 0) {
    cniElementName(&rc, element, (CciChar*)&elementName, sizeof(elementName));
    firstChild = cniFirstChild(&rc, element);
    if (firstChild) {
      depth++;
      Traverse(firstChild);
    }
    nextSibling = cniNextSibling(&rc, element);
    if (nextSibling) {
      Traverse(nextSibling);
    }
    else {
      depth--;
    }
  }
  else {
    depth--;
  }

  return;
}

/******************************************************************************/
/*                                                                            */
/* Plugin Node Utility function:                                              */
/*                                                                            */
/*  Acquire storage for attribute and link to the context                     */
/*                                                                            */
/******************************************************************************/
ATTRIBUTE_TBL_ENTRY *insAttrTblEntry(
  NODE_CONTEXT_ST* context,
  CciChar*         attrName,
  int              type
){
  ATTRIBUTE_TBL_ENTRY* entry;
  size_t attrSize = 0;

  entry = (ATTRIBUTE_TBL_ENTRY *)malloc(sizeof(ATTRIBUTE_TBL_ENTRY));
  if (entry) {
    entry->next = 0;
    entry->type = type;

    switch (entry->type) {
    case CNI_TYPE_INTEGER:
      attrSize = 4;
      break;
    case CNI_TYPE_STRING:
      attrSize = MAX_STRING_SIZE;
    default:
      /* Error: Invalid datatype */
      break;
    }

    /* Acquire and clear storage for attribute value */
    entry->valptr = malloc(attrSize);
    if (entry->valptr) memset(entry->valptr, 0, attrSize);

    /* Store the attribute name */
    CciCharCpy(entry->name, attrName);

    /* Link an existing previous element to this one */
    if (context->attrListPrevious) context->attrListPrevious->next = entry;
    else if ((context->attrListHead) == 0) context->attrListHead = entry;

    /* Save the pointer to the previous element */
    context->attrListPrevious = entry;
  }
  else {
    /* Error: Unable to allocate memory */
  }

  return(entry);
}

/******************************************************************************/
/*                                                                            */
/* Plugin Node Utility function:                                              */
/*                                                                            */
/*  Return the terminal object for a named input terminal                     */
/*                                                                            */
/******************************************************************************/
void *getInputTerminalHandle(
  NODE_CONTEXT_ST* context,
  CciChar*         terminalName
){
  TERMINAL_LIST_ENTRY *p = ((NODE_CONTEXT_ST *)context)->inputTerminalListHead;

  /* Scan the list for a matching terminal name */
  while (p != 0) {
    if (CciCharCmp(p->name, terminalName) == 0) {
      /* Match found; return it */
      return(p->handle);
    }
    p = (TERMINAL_LIST_ENTRY *)p->next;
  }

  /* No match was found */
  return(0);
}

/******************************************************************************/
/*                                                                            */
/* Plugin Node Utility function:                                              */
/*                                                                            */
/*  Acquire storage for an input terminal entry and link it to the context    */
/*                                                                            */
/******************************************************************************/
TERMINAL_LIST_ENTRY *insInputTerminalListEntry(
  NODE_CONTEXT_ST* context,
  CciChar*         terminalName
){
  TERMINAL_LIST_ENTRY* entry;
  int                  rc;

  entry = (TERMINAL_LIST_ENTRY *)malloc(sizeof(TERMINAL_LIST_ENTRY));
  if (entry) {

    /* This entry is the current end of the list */
    entry->next = 0;

    /* Store the terminal name */
    CciCharCpy(entry->name, terminalName);

    /* Create terminal and save its handle */
    entry->handle = cniCreateInputTerminal(&rc, context->nodeObject, (CciChar*)terminalName);

    /* Link an existing previous element to this one */
    if (context->inputTerminalListPrevious) context->inputTerminalListPrevious->next = entry;
    else if ((context->inputTerminalListHead) == 0) context->inputTerminalListHead = entry;

    /* Save the pointer to the previous element */
    context->inputTerminalListPrevious = entry;
  }
  else {
    /* Error: Unable to allocate memory */
  }

  return(entry);
}

/******************************************************************************/
/*                                                                            */
/* Plugin Node Utility function:                                              */
/*                                                                            */
/*  Return the terminal object for a named output terminal                    */
/*                                                                            */
/******************************************************************************/
void *getOutputTerminalHandle(
  NODE_CONTEXT_ST* context,
  CciChar*         terminalName
){
  TERMINAL_LIST_ENTRY *p = ((NODE_CONTEXT_ST *)context)->outputTerminalListHead;

  /* Scan the list for a matching terminal name */
  while (p != 0) {
    if (CciCharCmp(p->name, terminalName) == 0) {

      /* Match found; return it */
      return(p->handle);
    }
    p = (TERMINAL_LIST_ENTRY *)p->next;
  }

  /* No match was found */
  return(0);
}

/******************************************************************************/
/*                                                                            */
/* Plugin Node Utility function:                                              */
/*                                                                            */
/*  Acquire storage for an output terminal and link it to the context         */
/*                                                                            */
/******************************************************************************/
TERMINAL_LIST_ENTRY *insOutputTerminalListEntry(
  NODE_CONTEXT_ST* context,
  CciChar*         terminalName
){
  TERMINAL_LIST_ENTRY* entry;
  int                  rc;

  entry = (TERMINAL_LIST_ENTRY *)malloc(sizeof(TERMINAL_LIST_ENTRY));
  if (entry) {

    /* This entry is the current end of the list */
    entry->next = 0;

    /* Store the terminal name */
    CciCharCpy(entry->name, terminalName);

    /* Create terminal and save its handle */
    entry->handle = cniCreateOutputTerminal(&rc, context->nodeObject, (CciChar*)terminalName);

    /* Link an existing previous element to this one */
    if (context->outputTerminalListPrevious) context->outputTerminalListPrevious->next = entry;
    else if ((context->outputTerminalListHead) == 0) context->outputTerminalListHead = entry;

    /* Save the pointer to the previous element */
    context->outputTerminalListPrevious = entry;
  }
  else {
    /* Error: Unable to allocate memory */
  }

  return(entry);
}

#ifdef __cplusplus
}
#endif
