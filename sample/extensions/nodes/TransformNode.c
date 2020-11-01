/* Includes */
/* <malloc.h> is not available on /390 and Mac OS X */
#if !defined(__MVS__) && !defined(__APPLE__)
  #include <malloc.h>
#endif
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TransformNode.h"
#include "Common.h"

const CciChar * constTransformTraceLocation;
const CciChar * constTransformNode;

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                                                                            */
/* Plugin Node Utility function:                                              */
/*                                                                            */
/*  Node definition - ComIbmSampleTransformNode                               */
/*                                                                            */
/******************************************************************************/
void defineTransformNode(
  void* factoryObject
){
  static CNI_VFT vftable = {CNI_VFT_DEFAULT};
  int            rc = 0;

  /* Setup function table with pointers to node implementation functions */
  vftable.iFpCreateNodeContext = transform_createNodeContext;
  vftable.iFpDeleteNodeContext = _deleteNodeContext;
  vftable.iFpGetAttributeName  = _getAttributeName;
  vftable.iFpSetAttribute      = _setAttribute;
  vftable.iFpGetAttribute      = _getAttribute;
  vftable.iFpEvaluate          = transform_evaluate;

  /* Define a node type supported by our factory. If any errors/exceptions    */
  /* occur during the execution of this utility function, then as we have not */
  /* supplied the returnCode argument, the exception will bypass the plugin   */
  /* and be directly handled by the broker.                                   */
  cniDefineNodeClass(&rc, factoryObject, (CciChar*)constTransformNode, &vftable);

  return;
}

/******************************************************************************/
/*                                                                            */
/* Plugin Node Implementation Function:                cniCreateNodeContext() */
/*                                                                            */
/******************************************************************************/
CciContext* transform_createNodeContext(
  CciFactory* factoryObject,
  CciChar*    nodeName,
  CciNode*    nodeObject
){
  NODE_CONTEXT_ST* p;
  CciChar          buffer[256];

  /* Allocate a pointer to the local context */
  p = (NODE_CONTEXT_ST *)malloc(sizeof(NODE_CONTEXT_ST));

  if (p) {

    /* Clear the context area */
    memset(p, 0, sizeof(NODE_CONTEXT_ST));

    /* Save our node object pointer in our context */
    p->nodeObject = nodeObject;

    /* Save our node name */
    CciCharNCpy((CciChar*)&p->nodeName, nodeName, MAX_NODE_NAME_LEN);

    /* Create attributes and set default values */
    {
      const CciChar* ucsAttr = CciString("nodeTraceSetting", BIP_DEF_COMP_CCSID) ;
      insAttrTblEntry(p, (CciChar*)ucsAttr, CNI_TYPE_INTEGER);
      _setAttribute(p, (CciChar*)ucsAttr, (CciChar*)constZero);
      free((void *)ucsAttr) ;
    }
    {
      const CciChar* ucsAttr = CciString("nodeTraceOutfile", BIP_DEF_COMP_CCSID) ;
      insAttrTblEntry(p, (CciChar*)ucsAttr , CNI_TYPE_STRING);
      _setAttribute( p, (CciChar*)ucsAttr,
                    (CciChar*)constTransformTraceLocation);
      free((void *)ucsAttr) ;
    }

    /* Create terminals */
    {
      const CciChar* ucsIn = CciString("in", BIP_DEF_COMP_CCSID) ;
      insInputTerminalListEntry(p, (CciChar*)ucsIn);
      free((void *)ucsIn) ;
    }
    {
      const CciChar* ucsOut = CciString("out", BIP_DEF_COMP_CCSID) ;
      insOutputTerminalListEntry(p, (CciChar*)ucsOut);
      free((void *)ucsOut) ;
    }
    {
      const CciChar* ucsFailure = CciString("failure", BIP_DEF_COMP_CCSID) ;
      insOutputTerminalListEntry(p, (CciChar*)ucsFailure);
      free((void *)ucsFailure) ;
    }

    /* Get the value of this attribute */
    _getAttribute(p, (CciChar*)constNodeTraceOutfile, buffer, sizeof(buffer));

    /* Initialise default trace setting */
    p->trace = 1; 

    /* If tracing, open the file */
    if (p->trace) {
      static const char* functionName = "transform_createNodeContext()";
      const char * filename = mbString(buffer, BIP_DEF_COMP_CCSID);
      const char *mbNodeName = mbString(p->nodeName, BIP_DEF_COMP_CCSID);
      p->tracefile = fopen(filename, "w+");
      fprintf(p->tracefile, "NODE: <%s> -> %s factoryObject=0x%p\n",
              mbNodeName, functionName, factoryObject);
      fflush(p->tracefile);
      free((void *)filename);
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
/* Plugin Node Implementation Function:                         cniEvaluate() */
/*                                                                            */
/******************************************************************************/
void transform_evaluate(
  CciContext* context,
  CciMessage* destinationList,
  CciMessage* exceptionList,
  CciMessage* message
){
  CciMessage*      outMsg;
  CciTerminal*     terminalObject;
  CciElement*      bodyChild;
  CciElement*      firstChild;
  CciElement*      inRootElement;
  CciElement*      lastChild;
  CciElement*      outRootElement;
  int              retvalue;
  int              rc = 0;
  CCI_EXCEPTION_ST exception_st = {CCI_EXCEPTION_ST_DEFAULT};
  CciChar          elementName[256];

  /* Get the root element of the input message */
  inRootElement = cniRootElement(&rc, message);
  /*CCI_CHECK_RC();*/
  checkRC(rc);

  /* Create a new message for output */
  outMsg = cniCreateMessage(&rc, cniGetMessageContext(&rc, message));
  /*CCI_CHECK_RC();*/
  checkRC(rc);

  /* Get the root element of the output message */
  outRootElement = cniRootElement(&rc, outMsg);
  /*CCI_CHECK_RC();*/
  checkRC(rc);

  /* Copy the contents of the input message to the output message */
  cniCopyElementTree(&rc, inRootElement, outRootElement);
  /*CCI_CHECK_RC();*/
  checkRC(rc);

  /* Get the last child of root (ie the body) and it's name */
  bodyChild = cniLastChild(&rc, outRootElement);
  /*CCI_CHECK_RC();*/
  checkRC(rc);

  cniElementName(&rc, bodyChild, (CciChar*)&elementName, sizeof(elementName));
  /*CCI_CHECK_RC();*/
  checkRC(rc);

  /* Navigate to the root of the XML message, that is <Request> */
  lastChild = cniFirstChild(&rc, bodyChild);
  /*CCI_CHECK_RC();*/
  checkRC(rc);

  cniElementName(&rc, lastChild, (CciChar*)&elementName, sizeof(elementName));
  /*CCI_CHECK_RC();*/
  checkRC(rc);

  /* Navigate to the first child, that is the "type" attribute */
  firstChild = cniFirstChild(&rc, lastChild);
  /*CCI_CHECK_RC();*/
  checkRC(rc);

  cniElementName(&rc, firstChild, (CciChar*)&elementName, sizeof(elementName));
  /*CCI_CHECK_RC();*/
  checkRC(rc);

  /* If the type element, transform it's value to "modify" */
  if (CciCharCmp(elementName, (CciChar*)constType) == 0) {
    cniSetElementCharacterValue(&rc, firstChild, (CciChar*)constModify, 6);
    /*CCI_CHECK_RC();*/
    checkRC(rc);
  }

  /* Get handle of output terminal */
  terminalObject = getOutputTerminalHandle( (NODE_CONTEXT_ST *)context,
                                            (CciChar*)constOut);

  /* If the terminal exists and is attached, propagate to it */
  if (terminalObject) {
    if (cniIsTerminalAttached(&rc, terminalObject)) {
      /* As this is a new, and changed message, it should be finalized... */
      cniFinalize(&rc, outMsg, CCI_FINALIZE_NONE);
      retvalue = cniPropagate(&rc, terminalObject, destinationList, exceptionList, outMsg);
      if (retvalue == CCI_FAILURE) {
        if (rc == CCI_EXCEPTION) {
          /* Get details of the exception */
          memset(&exception_st, 0, sizeof(exception_st));
          cciGetLastExceptionData(&rc, &exception_st);

          /* Any local error handling may go here */

          /* Ensure message is deleted prior to return/throw */
          cniDeleteMessage(0, outMsg);

          /* We must "rethrow" the exception; note this does not return */
          cciRethrowLastException(&rc);
        }
        else {

          /* Some other error...the plugin might choose to log it using the CciLog() */
          /* utility function                                                        */

        }
      }
      else {
      }
    }
  }
  else {
    /* Terminal did not exist...severe internal error. The plugin may wish to */
    /* log an error here using the cciLog() utility function.                 */
  }

  /* Delete the message we created now we have finished with it */
  cniDeleteMessage(0, outMsg);

  return;
}

#ifdef __cplusplus
}
#endif
