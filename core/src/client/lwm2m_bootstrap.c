/************************************************************************************************************************
 Copyright (c) 2016, Imagination Technologies Limited and/or its affiliated group companies.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 following conditions are met:
     1. Redistributions of source code must retain the above copyright notice, this list of conditions and the
        following disclaimer.
     2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
        following disclaimer in the documentation and/or other materials provided with the distribution.
     3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
        products derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************************************************************/


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "coap_abstraction.h"
#include "lwm2m_endpoints.h"
#include "lwm2m_object_store.h"
#include "lwm2m_bootstrap.h"
#include "lwm2m_objects.h"
#include "lwm2m_security_object.h"
#include "lwm2m_server_object.h"
#include "lwm2m_bootstrap_config.h"
#include "lwm2m_result.h"

#define BOOTSTRAP_TIMEOUT          (10000)
#define BOOTSTRAP_FINISHED_TIMEOUT (15000)


static void Lwm2m_HandleBootstrapResponse(void * ctxt, AddressType* address, const char * responsePath, int responseCode, ContentType contentType, char * payload, int payloadLen);


/*
 * The bootstrap interface is used to optionally configure a LWM2M Client so that it can successfully register with a LWM2M Server.
 * The client bootstrap operation is performed by sending a CoAP POST request to the LWM2M Bootstrap Server at the /bs path including
 * the Endpoint Client Name as a query string parameter.
 *
 * In client-initiated bootstrap, when the Bootstrap Server receives Request Bootstrap operation, the Bootstrap Server performs Write
 * and/or Delete operation. In server initiated bootstrap, the Bootstrap Server performs Write operation.
 * The Write or Delete operation targets to an Object Instance or a Resource. The Write and Delete operation can be sent multiple times.
 * Only in Bootstrap Interface, Delete operation MAY target to “/” URI to delete all the existing Object Instances except
 * LWM2M Bootstrap Server Account in the LWM2M Client for initializing before LWM2M Bootstrap Server sends Write operation(s)
 * to the LWM2M Client. Different from „Write“ operation in Device Management and Service Enablement interface, the LWM2M Client MUST write
 * the value included in the payload regardless of an existence of the targeting Object Instance or Resource.
 */
static void Lwm2m_SendBootStrapRequest(Lwm2mContextType * context, int shortServerID)
{
    char * uriPath = "/bs";
    char uriQuery[128];
    char buffer[128];
    char serverPath[128];
    char uri[1024];

    Lwm2mCore_GetEndPointClientName(context, buffer, sizeof(buffer));
    sprintf(uriQuery, "?ep=%s", buffer);

    Lwm2m_GetServerURI(context, shortServerID, serverPath, sizeof(serverPath));

    sprintf(uri, "%s%s%s", serverPath, uriPath, uriQuery);
    Lwm2m_Info("Bootstrap %s\n", uri);

    coap_PostRequest(context, uri, ContentType_None, NULL, 0, Lwm2m_HandleBootstrapResponse);
}

static void Lwm2m_HandleBootstrapResponse(void * ctxt, AddressType* address, const char * responsePath, int responseCode, ContentType contentType, char * payload, int payloadLen)
{
    Lwm2mContextType * context = ctxt;

    if (responseCode == 204)
    {
        Lwm2m_Info("Waiting for bootstrap finished\n");
        context->BootStrapState = Lwm2mBootStrapState_BootStrapFinishPending;
        context->LastBootStrapUpdate = Lwm2mCore_GetTickCountMs();
    }
    else
    {
        context->BootStrapState = Lwm2mBootStrapState_BootStrapFailed;
    }
}

static bool Lwm2m_BootStrapFromSmartCard(Lwm2mContextType * context)
{
    // not implemented

    Lwm2m_Debug("Lwm2m_BootstrapFromSmartCard\n");
    return false;
}

// Attempt to retrieve the bootstrap information from factory configuration file
static bool Lwm2m_BootStrapFromFactory(Lwm2mContextType * context)
{
    bool rc = false;

#ifndef CONTIKI
    Lwm2m_Debug("Lwm2m_BootstrapFromFactory: %s\n", context->FactoryBootstrapInformation != NULL ? context->FactoryBootstrapInformation : "Skipped");

    if (context->FactoryBootstrapInformation != NULL)
    {
        const BootstrapInfo * bootstrapInfo = BootstrapInformation_ReadConfigFile(context->FactoryBootstrapInformation);
        if (bootstrapInfo == NULL || BootstrapInformation_WriteToObjectStore(context, bootstrapInfo) != 0)
        {
            Lwm2m_Error("Factory Bootstrap failed\n");
            Lwm2m_Error("EXITING\n");
            exit(1);
        }
        else
        {
            Lwm2m_Info("Factory Bootstrap from %s\n", context->FactoryBootstrapInformation);
            BootstrapInformation_Dump(bootstrapInfo);
            rc = true;
        }
        BootstrapInformation_DeleteBootstrapInfo(bootstrapInfo);
    }
#endif
    return rc;
}

// Handler called when the server posts a "finished" message to /bs
static int Lwm2m_BootStrapPost(void * ctxt, AddressType * addr, const char * path, const char * query, ContentType contentType,
                               const char * requestContent, int requestContentLen, char * responseContent, int * responseContentLen, int * responseCode)
{
    Lwm2mContextType * context = (Lwm2mContextType *)ctxt;

    *responseContentLen = 0;  // no content

    Lwm2m_Debug("POST to /bs\n");

    if (context->BootStrapState == Lwm2mBootStrapState_BootStrapFinishPending)
    {
        Lwm2m_Info("Bootstrap finished\n");
        context->BootStrapState = Lwm2mBootStrapState_BootStrapped;
        Lwm2mCore_UpdateAllServers(context, Lwm2mRegistrationState_Register);
        *responseCode = 204;
    }
    else if ((context->BootStrapState == Lwm2mBootStrapState_BootStrapPending) ||
             (context->BootStrapState == Lwm2mBootStrapState_ClientHoldOff))
    {
        Lwm2m_Info("Server initiated bootstrap\n");
        context->BootStrapState = Lwm2mBootStrapState_BootStrapped;
        Lwm2mCore_UpdateAllServers(context, Lwm2mRegistrationState_Register);
        *responseCode = 204;
    }
    else
    {
        *responseCode = 400;
    }
    return 0;
}

// Handler for /bs
static int Lwm2mCore_BootstrapEndpointHandler(int type, void * ctxt, AddressType * addr,
                                              const char * path, const char * query, const char * token, int tokenLength,
                                              ContentType contentType, const char * requestContent, int requestContentLen,
                                              ContentType * responseContentType, char * responseContent, int * responseContentLen, int * responseCode)
{
    switch (type)
    {
        case COAP_PUT_REQUEST:  // no break
        case COAP_POST_REQUEST:
            return Lwm2m_BootStrapPost(ctxt, addr, path, query, contentType, requestContent, requestContentLen, responseContent, responseContentLen, responseCode);
        default:
            *responseContentType = ContentType_None;
            *responseContentLen = 0;
            *responseCode = Lwm2mResult_MethodNotAllowed;
            break;
    }
    return -1;
}

// Initialise the boot strap mechanism, create the /bs endpoint
void Lwm2m_BootStrapInit(Lwm2mContextType * context)
{
    Lwm2mCore_AddResourceEndPoint(&context->EndPointList, "/bs", Lwm2mCore_BootstrapEndpointHandler);
}

/* The LWM2M Client MUST follow the procedure specified as below when attempting to bootstrap a LWM2M Device:
 * 1. If the LWM2M Device has Smartcard, the LWM2M Client tries to obtain Bootstrap Information
 *    from the Smartcard using the Bootstrap from Smartcard mode.
 * 2. If the LWM2M Client is not configured using the Bootstrap from Smartcard mode, the LWM2M
 *    Client tries to obtain the Bootstrap Information by using Factory Bootstrap mode.
 * 3. If the LWM2M Client has any LWM2M Server Object Instances from the previous steps, the LWM2M
 *    Client tries to register to the LWM2M Server(s) configured in the LWM2M Server Object Instance(s).
 * 4. If LWM2M Client fails to register to all the LWM2M Servers or the Client doesn’t have any
 *    LWM2M Server Object Instances, and the LWM2M Client hasn’t received a Server Initiated Bootstrap
 *    within the ClientHoldOffTime, the LWM2M Client performs the Client Initiated Bootstrap.
 */
void Lwm2m_UpdateBootStrapState(Lwm2mContextType * context)
{
    uint32_t now = Lwm2mCore_GetTickCountMs();
    uint32_t clientHoldOff;
    enum { SERVER_BOOTSTRAP = 0 };

    switch (context->BootStrapState)
    {
        case Lwm2mBootStrapState_NotBootStrapped:

            // First attempt smart card bootstrap.
            if (Lwm2m_BootStrapFromSmartCard(context))
            {
                context->BootStrapState = Lwm2mBootStrapState_BootStrapped;
            }
            // If that fails try and use the factory information.
            else if (Lwm2m_BootStrapFromFactory(context))
            {
                context->BootStrapState = Lwm2mBootStrapState_BootStrapped;
            }
            // If that fails wait for the client hold off time, for a server initiated bootstrap.
            else
            {
                Lwm2m_Info("Try existing server entries\n");
                context->BootStrapState = Lwm2mBootStrapState_CheckExisting;
            }
            Lwm2mCore_UpdateAllServers(context, Lwm2mRegistrationState_Register);
            context->LastBootStrapUpdate = now;
            break;

        case Lwm2mBootStrapState_CheckExisting:
            // Pass control to Registration process, if this fails then we will
            // drop into the ClientHoldOff State
            break;

        case Lwm2mBootStrapState_ClientHoldOff:
            // Wait for server initiated bootstrap.
            // Only one bootstrap server is supported.

            // If the hold off timer has expired, then request a boot strap.
            Lwm2m_GetClientHoldOff(context, SERVER_BOOTSTRAP, &clientHoldOff);

            if (now - context->LastBootStrapUpdate >= (clientHoldOff * 1000))
            {
                Lwm2m_Info("HoldOff Expired - Attempt Client Bootstrap\n");
                context->BootStrapState = Lwm2mBootStrapState_BootStrapPending;
                Lwm2m_SendBootStrapRequest(context, SERVER_BOOTSTRAP);
                context->LastBootStrapUpdate = now;
            }
            break;

        case Lwm2mBootStrapState_BootStrapPending:

            if (now - context->LastBootStrapUpdate >= BOOTSTRAP_TIMEOUT)
            {
                Lwm2m_Error("No response to client initiated bootstrap\n");
                context->BootStrapState = Lwm2mBootStrapState_BootStrapFailed;
                context->LastBootStrapUpdate = now;
            }

            break;

        case Lwm2mBootStrapState_BootStrapFinishPending:
            // The 2015/07/07 LWM2M draft requires that the server sends a bootstrap finished to the clients /bs endpoint,
            // however for now lets just wait up to 15 seconds and then move to BootStrapped state.
            if (now - context->LastBootStrapUpdate >= BOOTSTRAP_FINISHED_TIMEOUT)
            {
                Lwm2m_Warning("No bootstrap finished after 15 seconds, retrying...\n");
                context->BootStrapState = Lwm2mBootStrapState_BootStrapFailed;
            }
            break;

        case Lwm2mBootStrapState_BootStrapped:
            break;

        case Lwm2mBootStrapState_BootStrapFailed:

            // If the hold off timer has expired, then request a boot strap.
            Lwm2m_GetClientHoldOff(context, SERVER_BOOTSTRAP, &clientHoldOff);

            if (now - context->LastBootStrapUpdate >= (clientHoldOff * 1000))
            {
                Lwm2m_Warning("HoldOff Expired - Re-attempt boot-strap\n");
                context->BootStrapState = Lwm2mBootStrapState_NotBootStrapped;
                Lwm2m_SendBootStrapRequest(context, SERVER_BOOTSTRAP);
            }
            break;

        default:
            Lwm2m_Error("Unhandled bootstrap state %d\n", context->BootStrapState);
    }
}
