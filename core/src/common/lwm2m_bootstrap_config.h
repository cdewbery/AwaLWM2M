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


#ifndef BOOTSTRAP_INFORMATION_H
#define BOOTSTRAP_INFORMATION_H

#include "lwm2m_core.h"

#ifdef __cplusplus
extern "C" {
#endif

// Opaque container for bootstrap information
typedef struct _BootstrapInfo BootstrapInfo;

// Load bootstrap information from local filesystem into holding structures. Return allocated pointer to BootstrapInfo struct on success, NULL on error.
const BootstrapInfo * BootstrapInformation_ReadConfigFile(const char * configFile);

// Print bootstrap information to stdout
void BootstrapInformation_Dump(const BootstrapInfo * bootstrapInfo);

// Free the memory previously allocated by BootstrapInformation_ReadConfigFile()
void BootstrapInformation_DeleteBootstrapInfo(const BootstrapInfo * bootstrapInfo);

// Write bootstrap information to ObjectStore. Return 0 on success.
int BootstrapInformation_WriteToObjectStore(Lwm2mContextType * context, const BootstrapInfo * bootstrapInfo);

#ifdef __cplusplus
}
#endif

#endif // BOOTSTRAP_INFORMATION_H
