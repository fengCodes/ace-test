/*
 * Sample program for use with Product         
 *  ProgIds: 5724-J06 5724-J05 5724-J04 5697-J09 5655-M74 5655-M75 5648-C63
 *  (C) Copyright IBM Corporation 2004-2019.                     
 * All Rights Reserved * Licensed Materials - Property of IBM
 *
 * This sample program is provided AS IS and may be used, executed,
 * copied and modified without royalty payment by customer
 *
 * (a) for its own instruction and study,
 * (b) in order to develop applications designed to run with an IBM
 *     product, either for customer's own internal use or for
 *     redistribution by customer, as part of such an application, in
 *     customer's own products.
 */

package IntegrationAPI;

import java.util.List;

import com.ibm.integration.admin.model.ApplicationModel;
import com.ibm.integration.admin.model.IntegrationServerModel;
import com.ibm.integration.admin.model.MessageFlowModel;
import com.ibm.integration.admin.model.RestApiModel;
import com.ibm.integration.admin.proxy.ApplicationProxy;
import com.ibm.integration.admin.proxy.IntegrationAdminException;
import com.ibm.integration.admin.proxy.IntegrationNodeProxy;
import com.ibm.integration.admin.proxy.IntegrationServerProxy;
import com.ibm.integration.admin.proxy.MessageFlowProxy;
import com.ibm.integration.admin.proxy.RestApiProxy;

/*****************************************************************************
 * <P>An application to display information on top level Applications and Flows
 *  in a specified integration node.
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
 *****************************************************************************/
public class IntegrationNodeInfo {
    
    /**
     * Object that defines the connection to the integration node
     */
    IntegrationNodeProxy connectedInstance;
    
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
                
            } catch (IntegrationAdminException e) {
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
    private IntegrationNodeProxy connect(String integrationNodeHost, int integrationNodePort) {
        IntegrationNodeProxy integrationNode = null;

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

        integrationNode = new IntegrationNodeProxy(integrationNodeHost, integrationNodePort,"","",false);
        return integrationNode;
    }

    /**
     * Connects to a local integration node
     * @param localIntegrationNodeName The host name of the integration node
     * May be null, in which case the default parameters will be used. 
     * @return IntegrationNodeProxy connected instance. If the connection
     * could not be established, null is returned.
     */
    private IntegrationNodeProxy connect(String localIntegrationNodeName) {
        IntegrationNodeProxy integrationNode = null;

        String integrationNodeName = localIntegrationNodeName;
        if(integrationNodeName == null) 
        {
          integrationNodeName = DEFAULT_INTEGRATIONNODE_NAME;
        }
        
        integrationNode = new IntegrationNodeProxy(integrationNodeName);
        
        return integrationNode;
    }

    /**
     * Outputs to the screen name and runstate information for
     * each integration server and message flow in the supplied integration 
     * node.
     * @param integrationNode Integration node for which information is to be displayed.
     * @throws IntegrationAdminException 
     */
    private void displayIntegrationNodeInfo(IntegrationNodeProxy integrationNode) throws IntegrationAdminException {        
        
        displayIntegrationNodeRunstate(integrationNode);
        
        // Get all the integration servers in this integration node
        List<IntegrationServerProxy> allIntegrationServersInThisIntegrationNode = integrationNode.getAllIntegrationServers();
        if (!allIntegrationServersInThisIntegrationNode.isEmpty()){
            for (IntegrationServerProxy integrationServerProxy : allIntegrationServersInThisIntegrationNode){
                displayIntegrationServerRunstate(integrationServerProxy);

                // get all Applications
                List<ApplicationProxy> appProxyList = integrationServerProxy.getAllApplications(true);
                if (!appProxyList.isEmpty()){
                    for (ApplicationProxy appProxy : appProxyList){
                        displayApplicationRunstate(appProxy);
                        
                        // Get all Flows in the Application
                        List<MessageFlowProxy> appFlowProxyList = appProxy.getAllMessageFlows(true);
                        if (!appFlowProxyList.isEmpty()){
                            for (MessageFlowProxy flowProxy : appFlowProxyList){
                                displayMessageFlowRunstate(flowProxy);
                            }
                        }
                        else {
                            System.out.println("No flows are deployed.");
                        }
                    }
                }
                else {
                    System.out.println("No applications are deployed.");
                }
                
                // get all rest apis
                List<RestApiProxy> restApiProxyList = integrationServerProxy.getAllRestApis(true);
                if (!restApiProxyList.isEmpty()){
                    for (RestApiProxy restApiProxy : restApiProxyList){
                        displayRestAPIRunstate(restApiProxy);
                    }
                }
                else {
                    System.out.println("No rest apis are deployed.");
                }
            }
        }
        else {
            System.out.println("No integration servers exist.");
        }
        
    }

    /**
     * Outputs to the screen a line of text describing the integration 
     * node and whether it is running 
     * @param thisIntegrationNode IntegrationNodeProxy object representing an integration node
     * @throws IntegrationAdminException 
     */
    private void displayIntegrationNodeRunstate(IntegrationNodeProxy thisIntegrationNode)
    throws IntegrationAdminException {
        
        boolean isRunning = thisIntegrationNode.getIntegrationNodeModel(true)!=null;
        String integrationNodeName = thisIntegrationNode.getName();
        if (isRunning) {
            System.out.println("Integration Node "+ integrationNodeName+" is running ");
        } else {
            System.out.println("Integration Node "+ integrationNodeName+" is stopped ");
        }
    }
    
    /**
     * Outputs to the screen a line of text describing the integration 
     * server and whether it is running 
     * @param thisIntegrationServer IntegrationServerProxy object representing
     * an integration server
     * @throws IntegrationAdminException 
     */
    private void displayIntegrationServerRunstate(IntegrationServerProxy thisIntegrationServer) throws IntegrationAdminException{
        
        IntegrationServerModel integrationServerModel = thisIntegrationServer.getIntegrationServerModel(true);
        String integrationServerName = integrationServerModel.getName();
        if (integrationServerModel.getActive().isRunning()){
            System.out.println("  Integration Server: " + integrationServerName + " is running.");
        }
        else {
            System.out.println("  integration Server: " + integrationServerName + " is stopped.");
        }

    }
    
    /**
     * Outputs to the screen a line of text describing a deployed
     * application and whether it is running
     * @param thisApp ApplicationProxy object representing an
     * application that has been deployed to an integration server
     * @throws IntegrationAdminException 
     */
    private void displayApplicationRunstate(ApplicationProxy thisApp) throws IntegrationAdminException {
        ApplicationModel applicationModel = thisApp.getApplicationModel(true);
        String applicationName = applicationModel.getName();
        if (applicationModel.getActive().isRunning()){
            System.out.println("    Application: " + applicationName + " is running.");
        }
        else {
            System.out.println("    Application: " + applicationName + " is stopped.");
        }
    }

    /**
     * Outputs to the screen a line of text describing a deployed
     * rest API and whether it is running
     * @param thisRestAPI RestAPIProxy object representing an
     * api that has been deployed to an integration server
     * @throws IntegrationAdminException 
     */
    private void displayRestAPIRunstate(RestApiProxy thisRestAPI) throws IntegrationAdminException {
        RestApiModel restApiModel = thisRestAPI.getRestApiModel(true);
        String restApiname = restApiModel.getName();
        if (restApiModel.getActive().isRunning()){
            System.out.println("    RestAPI:" + restApiname + " is running.");
        }
        else {
            System.out.println("    RestAPI: " + restApiname + " is stopped.");
        }
    }

    
    /**
     * Outputs to the screen a line of text describing a deployed
     * message flow and whether it is running
     * @param thisFlow MessageFlowProxy object representing a
     * message flow that has been deployed to an integration server
     * @throws IntegrationAdminException 
     */
    private void displayMessageFlowRunstate(MessageFlowProxy thisFlow) throws IntegrationAdminException  {
        MessageFlowModel flowModel = thisFlow.getMessageFlowModel(true);
        String flowName = flowModel.getName();
        if (flowModel.getActive().isRunning()){
            System.out.println("      Flow: " + flowName + " is running.");
        }
        else {
            System.out.println("      Flow: " + flowName + " is stopped.");
        }
    }
    
}
