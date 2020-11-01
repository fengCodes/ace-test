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

import com.ibm.integration.admin.model.common.LogEntry;
import com.ibm.integration.admin.model.server.DeployResult;
import com.ibm.integration.admin.proxy.IntegrationAdminException;
import com.ibm.integration.admin.proxy.IntegrationNodeProxy;
import com.ibm.integration.admin.proxy.IntegrationServerProxy;

/*****************************************************************************
 * <P>A simple application to deploy a BAR file.
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
 *****************************************************************************/
public class DeployBAR {
    
    /**
     * Attempts to deploy a BAR file 
     * <hostname> <port> <integrationServer> <barFile>
     * <localIntegrationNodeName> <integrationServer> <barFile>
     * @param args 
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
        String integrationServerName = "server1";
        String barFileName        = "sample.bar";
        // -----------------------------------------------
        if (args.length > 0) {
            if (args.length < 3) {
                System.err.println("Usage: Remote: DeployBAR [<hostname> <port> <integrationServer> <barFile>] or \n"
                        + "Local: DeployBAR [<localIntegrationNodeName> <integrationServer> <barFile>]");
                System.exit(1);
            } else {
                if (args.length == 3) {
                    integrationNodeHostName = args[0];
                    integrationServerName = args[1];
                    barFileName = args[2];
                    integrationNodePort=0;
                } else {
                    integrationNodeHostName = args[0];
                    integrationNodePort = Integer.parseInt(args[1]);
                    integrationServerName = args[2];
                    barFileName = args[3];
                }
            }
        }
        deployBAR(integrationNodeHostName, integrationNodePort, integrationServerName, barFileName);
    }
        
    private static void deployBAR(String integrationNodeHostName,
                                  int integrationNodePort,
                                  String integrationServerName,
                                  String barFileName) {
        IntegrationNodeProxy integrationNode = null;

        // Start communication with the integration node
        if (integrationNodePort==0) {
            System.out.println("Connecting to the integration node running at "+integrationNodeHostName+"...");

            integrationNode = new IntegrationNodeProxy(integrationNodeHostName); // local connection
        } else {
            System.out.println("Connecting to the integration node running at "+integrationNodeHostName+":"+integrationNodePort+"...");

            integrationNode = new IntegrationNodeProxy(integrationNodeHostName, integrationNodePort, "", "", false); //remote connection
        }
        
        try {
                
            System.out.println("Discovering integration server '"+integrationServerName+"'...");
            IntegrationServerProxy integrationServer = integrationNode.getIntegrationServerByName(integrationServerName);
            
            // If the integration server exists, deploy to it.
            if (integrationServer == null) {
                System.out.println("Integration server not found");
            } else {
                // Deploy the BAR file and display the result
                System.out.println("Deploying "+barFileName+"...");
                DeployResult deployResult = integrationServer.deploy(barFileName); // location of BAR
                LogEntry[] logEntries = deployResult.getLogEntry();
                boolean successDeploy = true;
                for (LogEntry logEntry : logEntries){
                    // Any errors then deploy has failed
                    if (logEntry.getMessage().getSeverityCode().equals("E")) {
                        successDeploy=false;
                    }
                    System.out.print("BIPMessage: "+logEntry.getMessage().getNumber()
                            +" FullText "+logEntry.getText()+"\n");
                }
                
                System.out.print("Deploy success = " + successDeploy);
                
            }
                
        } catch (IntegrationAdminException e) {
            e.printStackTrace();
        }
    }      
}
