/*
 * Sample program for use with Product         
 *  ProgIds: 5724-J06 5724-J05 5724-J04 5697-J09 5655-M74 5655-M75 5648-C63
 *  (C) Copyright IBM Corporation 2004-2019.                     
 * All Rights Reserved * Licensed Materials - Property of IBM
 *
 * This sample program is provided AS IS and may be used, executed,
 * copied and modified without royalty payment by customer
 * 
 * This sample uses the deprecated Admin API also know as CMP or BrokerProxy and
 * is here for backwards compatibility only.
 * 
 * The new IntegrationAPI sample should be used instead. 
 *
 * (a) for its own instruction and study,
 * (b) in order to develop applications designed to run with an IBM
 *     product, either for customer's own internal use or for
 *     redistribution by customer, as part of such an application, in
 *     customer's own products.
 */

package IntegrationAPI.deprecated;

import java.util.Enumeration;

import com.ibm.broker.config.proxy.ApplicationProxy;
import com.ibm.broker.config.proxy.BrokerConnectionParameters;
import com.ibm.broker.config.proxy.BrokerProxy;
import com.ibm.broker.config.proxy.ConfigManagerProxyException;
import com.ibm.broker.config.proxy.ConfigManagerProxyLoggedException;
import com.ibm.broker.config.proxy.ConfigManagerProxyPropertyNotInitializedException;
import com.ibm.broker.config.proxy.ExecutionGroupProxy;
import com.ibm.broker.config.proxy.IntegrationNodeConnectionParameters;
import com.ibm.broker.config.proxy.MessageFlowProxy;
import com.ibm.broker.config.proxy.RestApiProxy;

/*****************************************************************************
 * <P>An application to display information on top level Applications and Flows
 *  in a specified integration node.
 *  
 * This sample uses the deprecated Admin API also know as CMP or BrokerProxy and
 * is here for backwards compatibility only.
 * 
 * The new IntegrationAPI sample should be used instead. 
 *
 * <P><TABLE BORDER="1" BORDERCOLOR="#000000" CELLSPACING="0"
 * CELLPADDING="5" WIDTH="100%">
 * <TR>
 *   <TD COLSPAN="2" ALIGN="LEFT" VALIGN="TOP" BGCOLOR="#C0FFC0">
 *     <B><I>IntegratioNodeInfo</I></B><P>
 *   </TD>
 * </TR>
 * <TR>
 *   <TD WIDTH="18%" ALIGN="LEFT" VALIGN="TOP">Responsibilities</TD>
 *   <TD WIDTH="*" ALIGN="LEFT" VALIGN="TOP">
 *     <UL>
 *     <LI>Provides example code that shows how to use the
 *    IBM Integration API to display integration node information.
 *     </UL>
 *   </TD>
 * </TR>
 * <TR>
 * </TABLE>
 * @deprecated
 *****************************************************************************/
public class IntegrationNodeInfo {
    
    /**
     * Object that defines the connection to the integration node
     */
    BrokerProxy connectedInstance;
    
    /**
     * The default name of the integration node if it is not 
     * supplied 
     */
    private final static String DEFAULT_INTEGRATIONNODE_NAME = "TESTNODE_"+System.getProperty("user.name");
    
    /**
     * The default host name of the integration node if it is not 
     * supplied 
     */
    private final static String DEFAULT_INTEGRATIONNODE_HOSTNAME = "localhost";
    
    /**
     * The default port of the integration node if it is not 
     * supplied 
     */
    private final static int DEFAULT_INTEGRATIONNODE_PORT = 4414;
        
    /**
     * Main method. Starts an instance of the integration node 
     * information program. 
     */
    public static void main(String[] args) {
        
        boolean finished = false;
        String parameter = null;
        
        // Parse the command line arguments
        for (int i=0; i<args.length; i++) {
            if ((args[i].equals("-h")) ||
                       (args[i].equals("-help")) ||
                       (args[i].equals("-?")) ||
                       (args[i].equals("/h")) ||
                       (args[i].equals("/help")) ||
                       (args[i].equals("/?"))) {
                displayUsageInfo();
                finished = true;
            } else {
                // An unflagged parameter was supplied.
                if (parameter == null) {
                    parameter = args[i];
                } else {
                    // Unrecognized parameter
                    finished = true;
                    displayUsageInfo();
                }
            }
        }
        
        if (!finished) {
            IntegrationNodeInfo di = null;
            if(parameter != null) 
            {
              String integrationNodeHost = null;
              int    integrationNodePort = 0;
              String tokens[] = parameter.split(":",2);
              if(tokens.length == 2)
              {
                 integrationNodeHost = tokens[0];
                 integrationNodePort = Integer.parseInt(tokens[1]);
                di = new IntegrationNodeInfo(integrationNodeHost, integrationNodePort);
              }
            }

            if(di == null) 
            {
              di = new IntegrationNodeInfo(parameter);
            }

            if(di != null) 
            {
              di.go();       
            }
        }
    }

    /**
     * Connects to an integration node using the supplied parameters
     * and asks it to return complete information on it. 
     * @param nodeName The name of the local integration node
     * May be null, in which case the default parameters will be used. 
     */
    public IntegrationNodeInfo(String nodeName) {
        connectedInstance = connect(nodeName);
        
        if (connectedInstance == null) {
            displayUsageInfo();
        }
    }
    
    /**
     * Connects to an integration node using the supplied parameters
     * and asks it to return complete information on it. 
     * @param nodeHost The host name of the integration node
     * May be null, in which case the default parameters will be used. 
     * @param nodePort The web administration port number of the 
     * integration node. May be 0, in which the case the default 
     * parameters will be used. 
     */
    public IntegrationNodeInfo(String nodeHost, int nodePort) {
        connectedInstance = connect(nodeHost, nodePort);
        
        if (connectedInstance == null) {
            displayUsageInfo();
        }
    }
    
    /**
     * Displays the syntax for the IntegrationNodeInfo command.
     */
    private static void displayUsageInfo() {
        System.out.println("Pass through `integrationNodeHost:IntegrationNodePortNumber` for remote connection or "
                + "`integrationNodeName` for local connection");
    }

    /**
     * Displays information on objects in the integration node. If 
     * the IBM Integration API  could not connect to the 
     * integration node using the parameters described on the 
     * constructor, this method does nothing. 
     */
    private void go() {
        if (connectedInstance!=null) {
            try {
                displayIntegrationNodeInfo(connectedInstance);
                
                connectedInstance.disconnect();
            } catch (ConfigManagerProxyException e) {
                e.printStackTrace();
            }
        }
    }
    
    /**
     * Connects, using the parameters to the web administration port
     * on which an integration node is listening on the named host.
     * @param integrationNodeHost The host name of the integration node
     * May be null, in which case the default parameters will be used. 
     * @param integrationNodePort The web administration port number of the 
     * integration node 
     * @return BrokerProxy connected instance. If the connection
     * could not be established, null is returned.
     */
    private BrokerProxy connect(String integrationNodeHost, int integrationNodePort) {
        BrokerProxy b = null;

        String integrationNodeHostName = integrationNodeHost;
        int    integrationNodePortNumber = integrationNodePort;
        if(integrationNodeHostName == null) 
        {
          integrationNodeHostName = DEFAULT_INTEGRATIONNODE_HOSTNAME;
        }
        if(integrationNodePortNumber == 0) 
        {
          integrationNodePortNumber = DEFAULT_INTEGRATIONNODE_PORT;
        }

        BrokerConnectionParameters bcp = new IntegrationNodeConnectionParameters(integrationNodeHostName, integrationNodePortNumber);
        
        try {
            b = BrokerProxy.getInstance(bcp);
            
            // Ensure the integration node is actually talking to us.
            // (This step isn't necessary - and from v11 has no effect
            boolean IntegrationNodeIsResponding = b.hasBeenPopulatedByBroker(true);
            
        } catch (ConfigManagerProxyLoggedException e) {
        }
        return b;
    }

    /**
     * Connects to a local integration node
     * @param localIntegrationNodeName The host name of the integration node
     * May be null, in which case the default parameters will be used. 
     * @return BrokerProxy connected instance. If the connection
     * could not be established, null is returned.
     */
    private BrokerProxy connect(String localIntegrationNodeName) {
        BrokerProxy b = null;

        String integrationNodeName = localIntegrationNodeName;
        if(integrationNodeName == null) 
        {
          integrationNodeName = DEFAULT_INTEGRATIONNODE_NAME;
        }
        
        try {
            b = BrokerProxy.getLocalInstance(integrationNodeName);
            
            // Ensure the integration node is actually talking to us.
            // (This step isn't necessary - - and from v11 has no effect
            boolean integreationNodeIsResponding = b.hasBeenPopulatedByBroker(true);
            
        } catch (ConfigManagerProxyLoggedException e) {
        }
        return b;
    }

    /**
     * Outputs to the screen name and runstate information for
     * each integration server and message flow in the integration 
     * node. 
     * @param b Integration node for which information
     * is to be displayed.
     * @throws ConfigManagerProxyException
     * if communication problems with the integration node meant 
     * that the required information could not be determined. 
     */
    private void displayIntegrationNodeInfo(BrokerProxy b)
    throws ConfigManagerProxyException {        
        
        displayIntegrationNodeRunstate(b);
        
        // Get all the integration servers in this integration node
        Enumeration<ExecutionGroupProxy> allEGsInThisIntegrationNode = b.getExecutionGroups(null);
        while (allEGsInThisIntegrationNode.hasMoreElements()) {
            ExecutionGroupProxy thisEG = allEGsInThisIntegrationNode.nextElement();
            displayIntegrationServerRunstate(thisEG);
            
            // Get all applications in this integration server
            Enumeration<ApplicationProxy> allAppsInThisEG = thisEG.getApplications(null);
            while (allAppsInThisEG.hasMoreElements()) {
                ApplicationProxy thisApp = allAppsInThisEG.nextElement();
                displayApplicationRunstate(thisApp);
            }
            
            // Get all message flows in this integration server
            Enumeration<RestApiProxy> allRestAPIsInThisEG = thisEG.getRestApis(null);
            while (allRestAPIsInThisEG.hasMoreElements()) {
                RestApiProxy thisRestAPI = allRestAPIsInThisEG.nextElement();
                displayRestAPIRunstate(thisRestAPI);
            }

            
            // Get all message flows in this integration server
            Enumeration<MessageFlowProxy> allFlowsInThisEG = thisEG.getMessageFlows(null);
            while (allFlowsInThisEG.hasMoreElements()) {
                MessageFlowProxy thisFlow = allFlowsInThisEG.nextElement();
                displayMessageFlowRunstate(thisFlow);
            }
            
        }
        
    }

    /**
     * Outputs to the screen a line of text describing the integration 
     * node and whether it is running 
     * @param thisIntegrationNode BrokerProxy object representing an 
     *                   integration node
     * @throws ConfigManagerProxyPropertyNotInitializedException
     * if communication problems with the integration node meant 
     * that the required information could not be determined. 
     */
    private void displayIntegrationNodeRunstate(BrokerProxy thisIntegrationNode)
    throws ConfigManagerProxyPropertyNotInitializedException {
        
        boolean isRunning = thisIntegrationNode.isRunning();
        String integrationNodeName = thisIntegrationNode.getName();
        if (isRunning) {
            System.out.println("Integration Node Running "+ integrationNodeName);
        } else {
            System.out.println("Integration Node Stopped "+ integrationNodeName);
        }
    }
    
    /**
     * Outputs to the screen a line of text describing the integration 
     * server and whether it is running 
     * @param thisExecutionGroup ExecutionGroupProxy object representing
     * an integration server
     * @throws ConfigManagerProxyPropertyNotInitializedException
     * if communication problems with the integration node meant 
     * that the required information could not be determined. 
     * @throws ConfigManagerProxyLoggedException if the parent
     * could not be accessed.
     */
    private void displayIntegrationServerRunstate(ExecutionGroupProxy thisExecutionGroup)
    throws ConfigManagerProxyPropertyNotInitializedException, ConfigManagerProxyLoggedException {
        boolean isRunning = thisExecutionGroup.isRunning();
        String integrationServerName = thisExecutionGroup.getName();
        String integrationNodeName = thisExecutionGroup.getParent().getName();
        
        if (isRunning) {
            System.out.println("IntegrationServer Running "+integrationServerName);
        } else {
            System.out.println("Integration Server Stopped "+integrationServerName);
        }
    }
    
    /**
     * Outputs to the screen a line of text describing a deployed
     * application and whether it is running
     * @param thisApp ApplicationProxy object representing an
     * application that has been deployed to an integration server
     * @throws ConfigManagerProxyPropertyNotInitializedException
     * if communication problems with the integration node meant 
     * that the required information could not be determined. 
     * @throws ConfigManagerProxyLoggedException if the parent
     * object could not be discovered.
     */
    private void displayApplicationRunstate(ApplicationProxy thisApp)
    throws ConfigManagerProxyPropertyNotInitializedException, ConfigManagerProxyLoggedException {
        boolean isRunning = thisApp.isRunning();
        String applicationName = thisApp.getName();
        String executionGroupName = thisApp.getParent().getName();
        String integrationNodeName = thisApp.getParent().getParent().getName();
        
        if (isRunning) {
            System.out.println("Application Running "+applicationName);
        } else {
            System.out.println("Application Stopped " + applicationName);
        }
    }

    /**
     * Outputs to the screen a line of text describing a deployed
     * rest API and whether it is running
     * @param thisRestAPI RestAPIProxy object representing an
     * api that has been deployed to an integration server
     * @throws ConfigManagerProxyPropertyNotInitializedException
     * if communication problems with the integration node meant 
     * that the required information could not be determined. 
     * @throws ConfigManagerProxyLoggedException if the parent
     * object could not be discovered.
     */
    private void displayRestAPIRunstate(RestApiProxy thisRestAPI)
    throws ConfigManagerProxyPropertyNotInitializedException, ConfigManagerProxyLoggedException {
        boolean isRunning = thisRestAPI.isRunning();
        String restAPIName = thisRestAPI.getName();
        String executionGroupName = thisRestAPI.getParent().getName();
        String integrationNodeName = thisRestAPI.getParent().getParent().getName();
        
        if (isRunning) {
            System.out.println("RestAPI Running "+restAPIName);
        } else {
            System.out.println("RestAPI Stopped " + restAPIName);
        }
    }

    
    /**
     * Outputs to the screen a line of text describing a deployed
     * message flow and whether it is running
     * @param thisFlow MessageFlowProxy object representing a
     * message flow that has been deployed to an integration server
     * @throws ConfigManagerProxyPropertyNotInitializedException
     * if communication problems with the integration node meant 
     * that the required information could not be determined. 
     * @throws ConfigManagerProxyLoggedException if the parent
     * object could not be discovered.
     */
    private void displayMessageFlowRunstate(MessageFlowProxy thisFlow)
    throws ConfigManagerProxyPropertyNotInitializedException, ConfigManagerProxyLoggedException {
        boolean isRunning = thisFlow.isRunning();
        String messageFlowName = thisFlow.getName();
        String executionGroupName = thisFlow.getParent().getName();
        
        if (isRunning) {
            System.out.println("Flow Running "+messageFlowName);
        } else {
            System.out.println("Flow Stoppped "+messageFlowName);
        }
    }
    
}
