@echo off
setlocal
REM Usage:   iib_createqueues <queueManager> 
REM Where:          <queueManager> is the queue manager you wish to use with the IIB Node
SET QMGR=%1

SET IIB_GROUP=mqbrkrs

REM Checking of input parameters and environment
if ("%MQSI_FILEPATH%."==".") (
  echo "The mqsiprofile has not been run.  This can be done as follows: '<iib install>\server\bin\mqsiprofile'"
  goto END
)
if  "%1".=="". (
  echo Need to supply queue manager name
  goto HELP
)

REM Run the MQSC Script to define IIB queues
runmqsc %qmgr% < "%MQSI_FILEPATH%\sample\wmq\iib_queues_create.mqsc"

REM Set MQ authorities on queues
setmqaut -m %QMGR% -t qmgr -g %IIB_GROUP% +altusr +connect +inq +setall +chg +dsp +system

REM *** EDA Nodes
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.TIMEOUT.QUEUE -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.AGGR.REQUEST -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.AGGR.CONTROL -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.AGGR.REPLY -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.AGGR.TIMEOUT -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.AGGR.UNKNOWN -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.EDA.COLLECTIONS -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.EDA.EVENTS -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.SEQ.GROUP -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.SEQ.NUMBER -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.SEQ.EXPIRY -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp

REM *** HTTP Listener
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.WS.INPUT -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.WS.REPLY -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.WS.ACK -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp

REM *** SAP Adapter persistence
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.ADAPTER.PROCESSED -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.ADAPTER.FAILED -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.ADAPTER.INPROGRESS -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.ADAPTER.NEW -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.ADAPTER.UNKNOWN -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp

REM *** Pub-sub
setmqaut -m %QMGR% -t topic -n SYSTEM.BROKER.MB.TOPIC -g %IIB_GROUP% +pub +sub

REM Data-capture (Record-replay)
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.DC.RECORD -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.DC.BACKOUT -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.DC.RECORD -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.DC.AUTH -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp

REM Security Queue
setmqaut -m %QMGR% -t queue -n SYSTEM.BROKER.AUTH -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp

REM Other existing queues need updated permissions
setmqaut -m %QMGR% -t queue -n SYSTEM.ADMIN.COMMAND.QUEUE -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.DEFAULT.MODEL.QUEUE -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.SELECTION.EVALUATION.QUEUE -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.SELECTION.VALIDATION.QUEUE -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp
setmqaut -m %QMGR% -t queue -n SYSTEM.INTERNAL.REPLY.QUEUE -g %IIB_GROUP% +browse +get +inq +put +set +setall +passall +passid +chg +dsp


goto END

:HELP
echo.
echo Usage: iib_createqueues ^<queueManager^>
echo Where: ^<queueManager^> is the queue manager you wish to use with the IIB Node

:END
endlocal
