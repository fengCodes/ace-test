/*
 * Sample program for use with Product         
 *  ProgIds: 5724-J06 5724-J05 5724-J04 5697-J09 5655-M74 5655-M75 5648-C63
 *  (C) Copyright IBM Corporation 1999, 2001.                     
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

package com.ibm.jsamples;

import com.ibm.broker.plugin.*;
import java.io.*;

/**
 * Sample plugin node.
 * This node alters the content of the incoming message before passing it to
 * its output terminal.
 * A minimal test message for this node would be:
 * <data><action>add</action></data>
 */
public class JavaTransformPluginNode extends MbNode implements MbNodeInterface
{
    String _nodeTraceSetting = "";
    String _nodeTraceOutfile = "";
    PrintWriter traceWriter = null;

    /**
     * Transform node constructor.
     * This is where input and output terminal are created.
     */
    public JavaTransformPluginNode() throws MbException
    {
        createInputTerminal("in");
        createOutputTerminal("out");
        createOutputTerminal("failure");
    }

    /**
     * This static method is called by the framework to identify this node.
     * If this method is not supplied, a default name will be generated 
     * automatically based on the node's package/class name.  In this case
     * it would be 'ComIbmSamplesTranformNode'.
     */
    public static String getNodeName()
    {
        return "TransformNode";
    }

    /**
     * This evaluate message is called by the broker for each message passing
     * through the flow.  The message assembly is passed in with the 'assembly'
     * parameter.  It is possible for a node to have more than one input
     * terminal.  The terminal that the message has come in on is represented
     * by the 'in' parameter.
     */
    public void evaluate(MbMessageAssembly assembly, MbInputTerminal in)
        throws MbException
    {
        // The incoming message assembly and its embedded messages are read-only.
        // New objects must be created using the copy constructors
        MbMessage newMsg = new MbMessage(assembly.getMessage());
        MbMessageAssembly newAssembly = new MbMessageAssembly(assembly, newMsg);

        // Navigate to the relevant syntax element in the XML message
        MbElement rootElement = newAssembly.getMessage().getRootElement();
        MbElement switchElement =
            rootElement.getFirstElementByPath("/XML/data/action");

        // To aid debugging, text can be printed to stdout/stderr.
        // On NT this can be viewed by selecting 'Allow sevice to interact with
        // desktop' on the NT Services properties dialog.
        // On Unix set the environment variable MQSI_RUN_ATTACHED=1 before
        // starting the broker.
        if (_nodeTraceSetting.equals("debug"))
        {
            System.out.println("Element = " + switchElement.getName());
            System.out.println("Value = " + switchElement.getValue());
            if(traceWriter != null) {
                traceWriter.println("Element = " + switchElement.getName());
                traceWriter.println("Value = " + switchElement.getValue());
            }
        }

        // Change the value of an existing element
        String elementValue = (String) switchElement.getValue();
        if (elementValue.equals("add"))
            switchElement.setValue("change");
        else if (elementValue.equals("change"))
            switchElement.setValue("delete");
        else if (elementValue.equals("delete"))
            switchElement.setValue("hold");
        else
            switchElement.setValue("failure");

        if (_nodeTraceSetting.equals("debug"))
        {
            System.out.println("New value = " + switchElement.getValue());
        }

        // Add a new tag as a child of the switch tag
        MbElement tag =
            switchElement.createElementAsLastChild(
                MbElement.TYPE_NAME,
                "PreviousValue",
                elementValue);

        // Add an attribute to this new tag
        tag.createElementAsFirstChild(
            MbElement.TYPE_NAME_VALUE,
            "NewValue",
            switchElement.getValue());

        MbOutputTerminal out = getOutputTerminal("out");

        // Now propagate the message assembly.
        // If the terminal is not attached, an exception will be thrown.  The user
        // can choose to handle this exception, but it is not neccessary since
        // the framework will catch it and propagate the message to the failure
        // terminal, or if it not attached, rethrow the exception back upstream.
        if(_nodeTraceSetting.equals("debug") && traceWriter != null) {
            traceWriter.println("Changed value of field " + switchElement.getName() + 
                     " from " + elementValue + " to " + switchElement.getValue());
            traceWriter.flush();            
        }

        out.propagate(newAssembly);
    }

    public String toString()
    {
        return getName();
    }

    /* Attributes are defined for a node by supplying get/set methods.
     * The following two methods define an attribute 'nodeTraceSetting'.
     * The capitalisation follows the usual JavaBean property convention.
     */
    public String getNodeTraceSetting()
    {
        return _nodeTraceSetting;
    }

    public void setNodeTraceSetting(String nodeTraceSetting)
    {
        _nodeTraceSetting = nodeTraceSetting;
        if(traceWriter != null) {
            traceWriter.println("Attribute nodeTraceSetting set to: " + nodeTraceSetting);
        }                
    }

    public String getNodeTraceOutfile()
    {
        return _nodeTraceOutfile;
    }

    public void setNodeTraceOutfile(String nodeTraceOutfile)
    {
        _nodeTraceOutfile = nodeTraceOutfile;
        if(traceWriter != null) {
            traceWriter.close();
            traceWriter = null;
        }
        try {
                    traceWriter = new PrintWriter(new FileWriter(_nodeTraceOutfile));
            if(_nodeTraceSetting.equals("debug") && traceWriter != null) {
            traceWriter.println("Attribute nodeTraceOutfile set to: " + nodeTraceOutfile);
            traceWriter.flush();
            }
        }
        catch(IOException e) {
            System.out.println("Trace file could not be set: " + e);
        }

    }

    /* The following method is used to close the trace output file handle.
    */
        public void onDelete()
    {
        if( traceWriter != null) {
            traceWriter.close();
            traceWriter = null;
        }
    }



}
