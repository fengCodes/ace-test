
#!/bin/sh

# Usage:   iib_createqueues <queueManager> <iib group>
# Where:          <queueManager> is the queue manager you wish to use with the IIB Node
#                 <iib group>    is the primary group of the system account that will run the IIB Node.
qmgr=$1
iib_group=$2

help(){
	echo -e \\nUsage: \\tiib_createqueues \<queueManager\> \<iib group\>
	echo -e Where: \\t\<queueManager\> is the queue manager you wish to use with the IIB Node
	echo -e        \\t\<iib group\>    is the primary group of the system account that will run the IIB Node.
}

# Checking of input parameters and environment
if [ -z "$MQSI_FILEPATH" ]; then
  echo "The mqsiprofile has not been run.  This can be done as follows: '. <iib install>/server/bin/mqsiprofile'"
  exit 1
fi
if [ -z "$qmgr" ]; then
  echo "Need to supply queue manager name"
  help
  exit 1
fi
if [ -z "$iib_group" ]; then
  echo "Need to supply primary group for the IIB Node"
  help
  exit 1
fi 

# Run the MQSC Script to define IIB queues
runmqsc $qmgr < $MQSI_FILEPATH/sample/wmq/iib_queues_create.mqsc

# Set MQ authorities on queues
setmqaut -m $qmgr -t qmgr -g $iib_group +altusr +connect +inq +setall +chg +dsp +system

# EDA Nodes
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.TIMEOUT.QUEUE -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.AGGR.REQUEST -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.AGGR.CONTROL -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.AGGR.REPLY -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.AGGR.TIMEOUT -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.AGGR.UNKNOWN -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.EDA.COLLECTIONS -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.EDA.EVENTS -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.SEQ.GROUP -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.SEQ.NUMBER -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.SEQ.EXPIRY -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp

# HTTP Listener
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.WS.INPUT -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.WS.REPLY -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.WS.ACK -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp

# SAP Adapter persistence
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.ADAPTER.PROCESSED -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.ADAPTER.FAILED -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.ADAPTER.INPROGRESS -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.ADAPTER.NEW -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.ADAPTER.UNKNOWN -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp

# Pub-sub
setmqaut -m $qmgr -t topic -n SYSTEM.BROKER.MB.TOPIC -g $iib_group +pub +sub

# Data-capture (Record-replay)
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.DC.RECORD -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.DC.BACKOUT -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.DC.AUTH -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp

# Security Queue
setmqaut -m $qmgr -t queue -n SYSTEM.BROKER.AUTH -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp

# Other existing queues need updated permissions
setmqaut -m $qmgr -t queue -n SYSTEM.ADMIN.COMMAND.QUEUE -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.DEFAULT.MODEL.QUEUE -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.SELECTION.EVALUATION.QUEUE -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.SELECTION.VALIDATION.QUEUE -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m $qmgr -t queue -n SYSTEM.INTERNAL.REPLY.QUEUE -g $iib_group +browse +get +inq +put +set +setall +passall +passid +chg +dsp

