#ifndef TRANSFORM_NODE_H
#define TRANSFORM_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <BipCni.h>

/* Trace will appear in current directory, if a specific location   */
/* is required, the location string should be altered to reflect    */
/* the platform specific location.                                  */
#define CONST_TRANSFORM_TRACE_LOCATION       "BipSampPluginNode_Transform.trc"
extern const CciChar * constTransformTraceLocation;


#define CONST_TRANSFORM_NODE                 "TransformNode"
extern const CciChar * constTransformNode;


void transform_evaluate(
  CciContext* context,
  CciContext* destinationList,
  CciContext* exceptionList,
  CciContext* message
);

CciContext* transform_createNodeContext(
  CciFactory* factoryObject,
  CciChar*    nodeName,
  CciNode*    nodeObject
);

void defineTransformNode(
  void* factoryObject
);

#ifdef __cplusplus
}
#endif

#endif
