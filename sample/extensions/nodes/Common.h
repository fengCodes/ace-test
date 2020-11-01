#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <BipCni.h>
#include "BipSampPluginUtil.h"

#ifndef max
#define max(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef min
#define min(x,y) (((x)<(y)) ? (x) : (y))
#endif

#define CONST_NODE_TRACE_OUT_FILE            "nodeTraceOutfile"
extern const CciChar * constNodeTraceOutfile;


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
#define CONST_ZERO                           "0"
extern const CciChar * constZero;


#define CONST_TYPE                           "type"
extern const CciChar * constType;

#define CONST_MODIFY                         "modify"
extern const CciChar * constModify;

#define CONST_OUT                            "out"
extern const CciChar * constOut;

#define CONST_ADD                            "add"
extern const CciChar * constAdd;

#define CONST_CHANGE                         "change"
extern const CciChar * constChange;

#define CONST_DELETE                         "delete"
extern const CciChar * constDelete;

#define CONST_HOLD                           "hold"
extern const CciChar * constHold;

#define CONST_FAILURE                        "failure"
extern const CciChar * constFailure;

#define MAX_ATTR_NAME_LEN     128
#define MAX_NODE_NAME_LEN     128
#define MAX_STRING_SIZE       256
#define MAX_TERMINAL_NAME_LEN 256
#define CNI_TYPE_INTEGER        1
#define CNI_TYPE_STRING         2


#define CCI_CHECK_RC()                                                                        \
   if (rc != CCI_SUCCESS) {                                                                   \
     if (rc == CCI_EXCEPTION) cciRethrowLastException(0);                                     \
     else {                                                                                   \
       return;                                                                                \
     }                                                                                        \
   }

typedef struct attributeTblEntry {
  void*    next;                    /* Pointer to next entry */
  void*    valptr;                  /* Pointer to value */
  int      type;                    /* Value type */
  CciChar  name[MAX_ATTR_NAME_LEN]; /* Attribute name */
} ATTRIBUTE_TBL_ENTRY;

typedef struct terminalListEntry {
  void*        next;                        /* Pointer to next entry */
  CciTerminal* handle;                      /* Handle to terminal object */
  CciChar      name[MAX_TERMINAL_NAME_LEN]; /* Terminal name */
} TERMINAL_LIST_ENTRY;

typedef struct context {
  CciNode*             nodeObject;                  /* Handle for node object */
  ATTRIBUTE_TBL_ENTRY* attrListHead;                /* Head of linked list of attributes */
  ATTRIBUTE_TBL_ENTRY* attrListPrevious;            /* Previous entry in attribute list */
  TERMINAL_LIST_ENTRY* inputTerminalListHead;       /* Head of linked list of input terminal handles */
  TERMINAL_LIST_ENTRY* inputTerminalListPrevious;   /* Previous entry in input terminal list */
  TERMINAL_LIST_ENTRY* outputTerminalListHead;      /* Head of linked list of output terminal handles */
  TERMINAL_LIST_ENTRY* outputTerminalListPrevious;  /* Previous entry in output terminal list */
  int                  trace;                       /* Flag: Trace active? */
  FILE*                tracefile;                   /* Trace file stream */
  CciChar              nodeName[MAX_NODE_NAME_LEN]; /* Node name */
} NODE_CONTEXT_ST;

/* Function prototypes */

void _deleteNodeContext(
  CciContext* context
);

int _getAttributeName(
  CciContext* context,
  int         index,
  CciChar*    buffer,
  int         bufsize
);

int _setAttribute(
  CciContext* context,
  CciChar*    attrName,
  CciChar*    attrValue
);

int _getAttribute(
  CciContext* context,
  CciChar*    attrName,
  CciChar*    buffer,
  int         bufsize
);

ATTRIBUTE_TBL_ENTRY *insAttrTblEntry(
  NODE_CONTEXT_ST* context,
  CciChar*         attrName,
  int              type
);

void *getInputTerminalHandle(
  NODE_CONTEXT_ST* context,
  CciChar*         terminalName
);

TERMINAL_LIST_ENTRY *insInputTerminalListEntry(
  NODE_CONTEXT_ST* context,
  CciChar*         terminalName
);

void *getOutputTerminalHandle(
  NODE_CONTEXT_ST* context,
  CciChar*         terminalName
);

TERMINAL_LIST_ENTRY *insOutputTerminalListEntry(
  NODE_CONTEXT_ST* context,
  CciChar*         terminalName
);




#ifdef __cplusplus
}
#endif


#endif

