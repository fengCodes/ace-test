/*
 * Sample program for use with Product         
 *  ProgIds: 5724-J06 5724-J05 5724-J04 5697-J09 5655-M74 5655-M75 5648-C63
 *  (C) Copyright IBM Corporation 2004-2019.                     
 * All Rights Reserved * Licensed Materials - Property of IBM
 *
 * This sample program is provided AS IS and may be used, executed,
 * copied and modified without royalty payment by customer
 * 
 * This sample uses the IntegrationAPI
 *
 * (a) for its own instruction and study,
 * (b) in order to develop applications designed to run with an IBM
 *     product, either for customer's own internal use or for
 *     redistribution by customer, as part of such an application, in
 *     customer's own products.
 */

package IntegrationAPI;

import java.io.File;
import java.io.IOException;
import java.util.List;

import com.ibm.integration.admin.http.HttpClient;
import com.ibm.integration.admin.http.HttpResponse;
import com.ibm.integration.admin.model.ApplicationModel;
import com.ibm.integration.admin.model.IntegrationServerModel;
import com.ibm.integration.admin.model.MessageFlowModel;
import com.ibm.integration.admin.model.SharedLibraryModel;
import com.ibm.integration.admin.proxy.ApplicationProxy;
import com.ibm.integration.admin.proxy.IntegrationAdminException;
import com.ibm.integration.admin.proxy.IntegrationNodeProxy;
import com.ibm.integration.admin.proxy.IntegrationServerProxy;
import com.ibm.integration.admin.proxy.MessageFlowProxy;
import com.ibm.integration.admin.proxy.SharedLibraryProxy;

/*****************************************************************************
 * <P>An application to display information on specified integration node
 * and to stop and start applications and flows.
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
 *     <LI>Provides example code that shows how to use the IBM Integration API.
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
public class IntegrationNodeOperations {

    public static void main(String[] args) {

        /**********************************************************/
        /* Use IntegrationNodeProxy and IntegrationServerProxy    */
        /* to access the Integration Node and its servers         */
        /* Create a server and get a model representation for it. */
        /**********************************************************/

        System.out.println("**********************************************************");
        System.out.println("* Use IntegrationNodeProxy and IntegrationServerProxy    *");
        System.out.println("* to access the Integration Node and its servers         *");
        System.out.println("* Create a server and get a model representation for it. *");
        System.out.println("**********************************************************");

        // TODO: Edit to use your own integration node
        String nodeName = "TESTNODE_"+System.getProperty("user.name");
        String serverName = "server1";

        String intServerDetailsFromModel = null;
        String intServerDetailsFromHttpClient = null;

        // Get an IntegrationNodeProxy object for the local integration node
        IntegrationNodeProxy intNodeProxy = new IntegrationNodeProxy(nodeName);
        String barFilePath = System.getProperty("user.dir")+"/sampleBAR/sample.bar";

        try {

            // Create an integration server called "server1" and get the IntegrationServerProxy        
            System.out.println("\nCreating Integration Server: " + serverName + " for Integration Node: " + nodeName + "\n");
            IntegrationServerProxy intServerProxy = intNodeProxy.createIntegrationServer(serverName);            

            System.out.println("Get a IntegrationServerModel representation of " + serverName + " from IntegrationServerProxy\n");

            // Get the IntegrationServerModel object representing "server1"
            IntegrationServerModel integrationServerModel = intServerProxy.getIntegrationServerModel(true);                

            // Get the last HTTP response
            HttpResponse httpResponse =  intServerProxy.getLastHttpResponse();            
            System.out.println("Details from last response message:");
            System.out.println("===================================");
            System.out.println("URL: " + httpResponse.getUrlPath());
            System.out.println("Status Code: " + httpResponse.getStatusCode());
            System.out.println("Reason: " + httpResponse.getReason());
            intServerDetailsFromModel = httpResponse.getBody();

            System.out.println("");
            System.out.println("Attributes retrieved from the model representation of " + serverName);
            System.out.println("=============================================================");

            // Use the IntegrationServerModel to get information about the Integration Server
            String build_level = integrationServerModel.getDescriptiveProperties().getBuildLevel();
            String version = integrationServerModel.getDescriptiveProperties().getVersion();
            System.out.println("Using ACE build level :  " + build_level);
            System.out.println("Using ACE version :  " + version);

        } catch (IntegrationAdminException e) {
            e.printStackTrace();            
        } 

        /**********************************************************/
        /* Use HttpClient to also access the node and its servers */            
        /**********************************************************/

        System.out.println("\n");
        System.out.println("**********************************************************");
        System.out.println("* Use HttpClient to also access the node and its servers *");
        System.out.println("**********************************************************");

        try {

            HttpClient httpClient = new HttpClient(nodeName);    

            System.out.println("");

            // Get information about the node using a GET method call
            HttpResponse httpResponse = httpClient.getMethod("/apiv2/servers/server1?depth=4");

            System.out.println("Details from last response message:");
            System.out.println("===================================");
            System.out.println("URL: " + httpResponse.getUrlPath());
            System.out.println("Status Code: " + httpResponse.getStatusCode());
            System.out.println("Reason: " + httpResponse.getReason());

            intServerDetailsFromHttpClient = httpResponse.getBody();

            System.out.println("");
            if (intServerDetailsFromModel.equals(intServerDetailsFromHttpClient)){
                System.out.println("==> the same details can be retrieved using IntegrationServerProxy or HttpClient");
            }
            else {
                System.out.println("==> Different body was found which was unexpected.");
            }

            System.out.println("");
            // Deploy a BAR file using a POST method call
            File barFile = new File(barFilePath);                    
            String urlPath = "/apiv2/servers/" + serverName + "/deploy";
            System.out.println("Deploying " + barFile.getName());
            httpResponse = httpClient.postMethod(urlPath, barFile);

            if (httpResponse.getStatusCode() == 200){
                System.out.println("==>Deploy was successful.\n");
                System.out.println("Details from last response message:");
                System.out.println("===================================");
                System.out.println("URL: " + httpResponse.getUrlPath());
                System.out.println("Status Code: " + httpResponse.getStatusCode());
                System.out.println("Reason: " + httpResponse.getReason());
            }

            System.out.println("");

        } catch (IOException | InterruptedException e) {
            e.printStackTrace();            
        } 

        /**********************************************************/
        /* Use these objects to access deployed resources         */
        /* ApplicationProxy                                       */
        /* SharedLibraryProxy                                     */
        /**********************************************************/

        System.out.println("**********************************************************");
        System.out.println("* Use these objects to access deployed resources         *");
        System.out.println("* ApplicationProxy                                       *");
        System.out.println("* SharedLibraryProxy                                     *");
        System.out.println("**********************************************************");

        try {

            // Get an IntegrationServerProxy object for "server1"
            IntegrationServerProxy intServerProxy = intNodeProxy.getIntegrationServerByName(serverName);

            System.out.println("");
            List <ApplicationProxy> appProxyList = intServerProxy.getAllApplications(true);                    
            if (!appProxyList.isEmpty()){
                for (ApplicationProxy appProxy : appProxyList){
                    ApplicationModel appModel = appProxy.getApplicationModel(true);
                    String appName = appModel.getName();
                    if (appModel.getActive().isRunning()){
                        System.out.println("Application: " + appName + " is running.");
                    }
                    else {
                        System.out.println("Application: " + appName + " is stopped.");
                    }
                }
            }
            else {
                System.out.println("No applications are deployed.");
            }

            System.out.println("");
            List <SharedLibraryProxy> sharedLibraryProxyList = intServerProxy.getAllSharedLibraries(true);            
            if (!sharedLibraryProxyList.isEmpty()){
                for (SharedLibraryProxy sharedLibraryProxy : sharedLibraryProxyList){
                    SharedLibraryModel sharedLibraryModel = sharedLibraryProxy.getSharedLibraryModel(true);
                    String sharedLibraryName = sharedLibraryModel.getName();
                    System.out.println("Found Shared Library: " + sharedLibraryName);
                }
            }
            else {
                System.out.println("No Shared Libraries are deployed.");
            }

        } catch (IntegrationAdminException e) {
            e.printStackTrace();            
        } 

        /**********************************************************/
        /* Stop/start Applications and Message Flows.             */
        /**********************************************************/

        System.out.println("");
        System.out.println("**********************************************************");
        System.out.println("* Stop/start Applications and Message Flows.             *");
        System.out.println("**********************************************************");
        System.out.println("");

        try {

            // Get an IntegrationServerProxy object for "server1"
            IntegrationServerProxy intServerProxy = intNodeProxy.getIntegrationServerByName(serverName);

            // Get the ApplicationProxy for a specific application
            String appName = "HelloApp";
            ApplicationProxy appProxy = intServerProxy.getApplicationByName(appName, true);
            ApplicationModel appModel = appProxy.getApplicationModel(true);
            if (appModel.getActive().isRunning()){

                // Stop the Application
                System.out.println("Stopping Application " + appName + "...");
                int stop_rc = appProxy.stop();
                System.out.println("==> rc = " + stop_rc);
                System.out.println();

                // Refresh the model
                System.out.println("Refreshing Application Model");
                appModel = appProxy.getApplicationModel(true);
                if (appModel.getActive().isRunning()){
                    System.out.println("==> Application: " + appName + " is running.");
                }
                else {
                    System.out.println("==> Application: " + appName + " is stopped.");
                }
                System.out.println();

                // Start the Application
                System.out.println("Starting Application " + appName + "...");
                int start_rc = appProxy.start();
                System.out.println("==> rc = " + start_rc);
                System.out.println();

                // Refresh the model
                System.out.println("Refreshing Application Model");
                appModel = appProxy.getApplicationModel(true);              
                if (appModel.getActive().isRunning()){
                    System.out.println("==> Application: " + appName + " is running.");
                }
                else {
                    System.out.println("==> Application: " + appName + " is stopped.");
                }
                System.out.println();
            }

            String flowName = "HelloFlow";
            MessageFlowProxy flowProxy = appProxy.getMessageFlowByName(flowName, null, true);
            MessageFlowModel flowModel = flowProxy.getMessageFlowModel(true);

            if (flowModel.getActive().isRunning()){

                // Stop the Message Flow
                System.out.println("Stopping Message Flow " + flowName + "...");
                int stop_rc = flowProxy.stop();
                System.out.println("==> rc = " + stop_rc);
                System.out.println();

                // Refresh the model
                System.out.println("Refreshing Message Flow Model");
                flowModel = flowProxy.getMessageFlowModel(true);
                if (flowModel.getActive().isRunning()){
                    System.out.println("==> Message Flow: " + flowName + " is running.");
                }
                else {
                    System.out.println("==> Message Flow: " + flowName + " is stopped.");
                }
                System.out.println();

                // Start the Message Flow
                System.out.println("Starting Message Flow " + flowName + "...");
                int start_rc = flowProxy.start();
                System.out.println("==> rc = " + start_rc);
                System.out.println();

                // Refresh the model
                System.out.println("Refreshing Message Flow Model");
                flowModel = flowProxy.getMessageFlowModel(true);              
                if (flowModel.getActive().isRunning()){
                    System.out.println("==> Message Flow: " + flowName + " is running.");
                }
                else {
                    System.out.println("==> Message Flow: " + flowName + " is stopped.");
                }
                System.out.println();
            }

        } catch (IntegrationAdminException e) {
            e.printStackTrace();            
        } 

        System.out.println();
        System.out.println("Finished run");

    }

}
