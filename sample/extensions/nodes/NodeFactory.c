/* Includes */
/* <malloc.h> is not available on /390 and Mac OS X */
#if !defined(__MVS__) && !defined(__APPLE__)
  #include <malloc.h>
#endif
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NodeFactory.h"
#include "Common.h"
#include "SwitchNode.h"
#include "TransformNode.h"

const CciChar * constZero;
const CciChar * constType;
const CciChar * constModify;
const CciChar * constOut;
const CciChar * constAdd;
const CciChar * constChange;
const CciChar * constDelete;
const CciChar * constHold;
const CciChar * constFailure;
const CciChar * constNodeTraceOutfile;

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                                                                            */
/* Plugin Node Implementation Function:                              cniRun() */
/*                                                                            */
/* This is an example framework for a plugin input node. We simulate reading  */
/* data from an external source and attach the buffer containing that data to */
/* the CciMessage object, passing that into the message flow by invoking the  */
/* cniPropagate() function to send the message to the terminal named "out".   */
/******************************************************************************/
#define PLUGIN_BUFFER_SIZE 4096

int input_run(
  CciContext* context,
  CciMessage* destinationList,
  CciMessage* exceptionList,
  CciMessage* message
){
  void*        buffer;
  CciTerminal* terminalObject;
  int          rc = CCI_SUCCESS;
  int          rcDispatch = CCI_SUCCESS;
  char         xmlData[] = "<A>data</A>"; 

  /* Obtain data from external source. For the purpose of this example, which */
  /* does not actually interface to an external data source, we allocate a    */
  /* buffer on the heap and setting it's contents to simulate XML being read  */
  /* from the external source.                                                */
  buffer = malloc(PLUGIN_BUFFER_SIZE);
  if (buffer)
    memcpy(buffer, &xmlData, sizeof(xmlData)); 

  /* Attach the data buffer to the input message object */
  cniSetInputBuffer(&rc, message, buffer, PLUGIN_BUFFER_SIZE);

  /* This input node is capable of running on multiple threads, so we request */
  /* for a thread to be allocated from the thread pool associated with the    */
  /* message flow.                                                            */ 
  cniDispatchThread(&rcDispatch, ((NODE_CONTEXT_ST *)context)->nodeObject);

  /* Get handle of our output terminal */
  terminalObject = getOutputTerminalHandle( (NODE_CONTEXT_ST *)context,
                                            (CciChar*)constOut);

  /* If the terminal exists and is attached, propagate to it */
  if (terminalObject) {
    if (cniIsTerminalAttached(&rc, terminalObject)) {
      if (rc == CCI_SUCCESS) {
        cniPropagate(&rc, terminalObject, destinationList, exceptionList, message);
      }
    }

    /* If an exception occurred, then free the buffer and rethrow. The broker */
    /* then catches the exception, backs out any transaction and calls this   */
    /* implementation function again.                                         */
    if (rc == CCI_EXCEPTION) {
      free(buffer);
      cciRethrowLastException(&rc);
    }   
  }

  /* Free the buffer we acquired */
  free(buffer);

  /* Return to the broker indicating that any transaction is be committed. If */
  /* we were able to dispatch another thread, then the current thread gets    */
  /* returned to the pool by returning CCI_SUCCESS_RETURN. If another thread  */
  /* could not be dispatched, then we return CCI_SUCCESS_CONTINUE to indicate */  
  /* that we require this implementation function to be called immediately    */
  /* afterwards on the same thread.                                           */
  if (rcDispatch == CCI_NO_THREADS_AVAILABLE) return CCI_SUCCESS_CONTINUE;
  else return CCI_SUCCESS_RETURN;
}

/******************************************************************************/
/*                                                                            */
/* Plugin Node Utility function:                                              */
/*                                                                            */
/*  initNodeConstants                                                         */
/*                                                                            */
/*  Creates Cci constant strings from defined char * constant strings.        */
/*                                                                            */
/******************************************************************************/
void initNodeConstants(
  void
){
  constZero                   = CciString(CONST_ZERO, BIP_DEF_COMP_CCSID);
  constSwitchTraceLocation    = CciString(CONST_SWITCH_TRACE_LOCATION, BIP_DEF_COMP_CCSID);
  constTransformTraceLocation = CciString(CONST_TRANSFORM_TRACE_LOCATION, BIP_DEF_COMP_CCSID);
  constSwitchNode             = CciString(CONST_SWITCH_NODE, BIP_DEF_COMP_CCSID);
  constTransformNode          = CciString(CONST_TRANSFORM_NODE, BIP_DEF_COMP_CCSID);
  constPluginNodeFactory      = CciString(CONST_PLUGIN_NODE_FACTORY, BIP_DEF_COMP_CCSID);
  constNodeTraceOutfile       = CciString(CONST_NODE_TRACE_OUT_FILE, BIP_DEF_COMP_CCSID);
  constType                   = CciString(CONST_TYPE, BIP_DEF_COMP_CCSID);
  constModify                 = CciString(CONST_MODIFY, BIP_DEF_COMP_CCSID);
  constOut                    = CciString(CONST_OUT, BIP_DEF_COMP_CCSID);
  constAdd                    = CciString(CONST_ADD, BIP_DEF_COMP_CCSID);
  constChange                 = CciString(CONST_CHANGE, BIP_DEF_COMP_CCSID);
  constDelete                 = CciString(CONST_DELETE, BIP_DEF_COMP_CCSID);
  constHold                   = CciString(CONST_HOLD, BIP_DEF_COMP_CCSID);
  constFailure                = CciString(CONST_FAILURE, BIP_DEF_COMP_CCSID);
}

/******************************************************************************/
/*                                                                            */
/* Plugin Node Implementation Function:                                       */
/*                                                                            */
/* This function is called when the message broker loads the "lil" during the */
/* initialization of the broker engine.                                       */
/*                                                                            */
/* The responsibilities of the plug-in are to:                                */
/*  - create the node factory (or factories) which the "lil" supports, using  */
/*    the createNodeFactory() function, saving the returned pointer to the    */
/*    node factory for use at runtime.                                        */
/*  - define each node supported by the (or each) factory) and pass a pointer */
/*    to a function table which contains pointers to the node implementation  */
/*    functions.                                                              */
/*  - define the name of each attribute supported by each node, including a   */
/*    pointer to a validation function.                                       */
/*  - define input and output terminals supported by each node.               */
/*  - return the pointer to the factory, unless an error occurred, in which   */
/*    case zero should be returned.                                           */
/*                                                                            */
/******************************************************************************/
CciFactory LilFactoryExportPrefix * LilFactoryExportSuffix bipGetMessageflowNodeFactory(void)
{
  CciFactory*      factoryObject;
  int              rc = 0;
  CCI_EXCEPTION_ST exception_st = {CCI_EXCEPTION_ST_DEFAULT};

  /* Before we proceed we need to initialise all the static constants */
  /* that may be used by the plug-in.                                 */
  initNodeConstants();

  /* Create the Node Factory for this plug-in */
  factoryObject = cniCreateNodeFactory(0, (unsigned short *)constPluginNodeFactory);
  if (factoryObject == CCI_NULL_ADDR) {
    if (rc == CCI_EXCEPTION) {
      /* Get details of the exception */
      memset(&exception_st, 0, sizeof(exception_st));
      cciGetLastExceptionData(&rc, &exception_st);

      /* Any local error handling may go here */

      /* Rethrow the exception */
      cciRethrowLastException(&rc);
    }
    /* Any further local error handling can go here */
  }
  else {
    /* Define the nodes supported by this factory */
    defineSwitchNode(factoryObject);
    defineTransformNode(factoryObject);
  }

  /* Return address of this factory object to the broker */
  return(factoryObject);
}

#ifdef __cplusplus
}
#endif
