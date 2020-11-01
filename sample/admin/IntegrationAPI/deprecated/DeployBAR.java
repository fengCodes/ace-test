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

import java.io.IOException;

import com.ibm.broker.config.proxy.BrokerConnectionParameters;
import com.ibm.broker.config.proxy.BrokerProxy;
import com.ibm.broker.config.proxy.ConfigManagerProxyException;
import com.ibm.broker.config.proxy.DeployResult;
import com.ibm.broker.config.proxy.ExecutionGroupProxy;
import com.ibm.broker.config.proxy.IntegrationNodeConnectionParameters;

/*****************************************************************************
 * <P>A simple application to deploy a BAR file.
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
 *     <B><I>DeployBAR</I></B><P>
 *   </TD>
 * </TR>
 * <TR>
 *   <TD WIDTH="18%" ALIGN="LEFT" VALIGN="TOP">Responsibilities</TD>
 *   <TD WIDTH="*" ALIGN="LEFT" VALIGN="TOP">
 *     <UL>
 *     <LI>Provides example code that shows how to
 *     use the IBM Integration API to deploy a BAR file.
 *     </UL>
 *   </TD>
 * </TR>
 * <TR>
 *   <TD WIDTH="18%" ALIGN="LEFT" VALIGN="TOP">Internal Collaborators</TD>
 *   <TD WIDTH="*" ALIGN="LEFT" VALIGN="TOP">
 *     None
 *   </TD>
 * </TR>
 * </TABLE>
 * @deprecated
 *****************************************************************************/
public class DeployBAR {
    
    /**
     * Attempts to deploy a BAR file to
     * the resource whose name is hard-coded within the source. 
     * @param args Not used
     */
    public static void main(String[] args) {
        
        // Modify the values of these variables in order
        // to change the integration node, integration server and BAR
        // file settings used by this sample, or override
        // a new set on the command line using the usage
        // information below.
        // -----------------------------------------------
        String integrationNodeHostName     = "localhost";
        int    integrationNodePort         = 4414;
        String executionGroupName = "default";
        String barFileName        = "mybar.bar";
        int    timeoutSecs        = 10;
        // -----------------------------------------------
        if (args.length > 0) {
            if (args.length < 4) {
                System.err.println("Usage: StartDeployBAR [<hostname> <port> <integrationServer> <barFile> [<timeoutSecs>]]");
                System.exit(1);
            } else {
                integrationNodeHostName = args[0];
                integrationNodePort = Integer.parseInt(args[1]);
                executionGroupName = args[2];
                barFileName = args[3];
                if (args.length>4) timeoutSecs = Integer.parseInt(args[4]);
            }
        }
        deployBAR(integrationNodeHostName, integrationNodePort, executionGroupName, barFileName, timeoutSecs);
    }
        
    private static void deployBAR(String integrationNodeHostName,
                                  int integrationNodePort,
                                  String executionGroupName,
                                  String barFileName,
                                  int timeoutSecs) {
        // Instantiate an object that describes the connection
        // characteristics to the integration node.
        BrokerConnectionParameters bcp =
            new IntegrationNodeConnectionParameters(integrationNodeHostName, integrationNodePort);
        BrokerProxy b = null;
        
        try {
            // Start communication with the integration node
            System.out.println("Connecting to the integration node running at "+integrationNodeHostName+":"+integrationNodePort+"...");
            b = BrokerProxy.getInstance(bcp);
                
            System.out.println("Discovering integration server '"+executionGroupName+"'...");
            ExecutionGroupProxy eg = b.getExecutionGroupByName(executionGroupName);
            
            // If the integration server exists, deploy to it.
            if (eg == null) {
                System.out.println("Integration server not found");
            } else {
                // Deploy the BAR file and display the result
                System.out.println("Deploying "+barFileName+"...");
                try {
                    DeployResult deployResult = eg.deploy(barFileName, // location of BAR
                                                          true,        // incremental, i.e. don't empty the integration server first
                                                          timeoutSecs * 1000); // wait for integration node response
                                                          
                    System.out.println("Result = "+deployResult.getCompletionCode());

                    // You may like to improve this application by querying
                    // the deployResult for more information, particularly if
                    // deployResult.getCompletionCode() == CompletionCodeType.failure.
                    
                } catch (IOException ioEx) {
                    // e.g. if BAR file doesn't exist
                    ioEx.printStackTrace();
                }
                
            }
                
        } catch (ConfigManagerProxyException cmpEx) {
            cmpEx.printStackTrace();

        }
    }      
}
