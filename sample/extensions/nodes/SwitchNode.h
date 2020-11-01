#ifndef SWITCH_NODE_H
#define SWITCH_NODE_H

#ifdef __cplusplus
extern "C" {
#endif
#include <BipCni.h>
/* Trace will appear in current directory, if a specific location   */
/* is required, the location string should be altered to reflect    */
/* the platform specific location.                                  */
#define CONST_SWITCH_TRACE_LOCATION          "BipSampPluginNode_Switch.trc"
extern const CciChar * constSwitchTraceLocation;


#define CONST_SWITCH_NODE                    "SwitchNode"
extern const CciChar * constSwitchNode;


void switch_evaluate(
  CciContext* context,
  CciContext* destinationList,
  CciContext* exceptionList,
  CciContext* message
);

CciContext* switch_createNodeContext(
  CciFactory* factoryObject,
  CciChar*    nodeName,
  CciNode*    nodeObject
);

void defineSwitchNode(
  void* factoryObject
);

#ifdef __cplusplus
}
#endif

#endif
