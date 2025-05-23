;==============================================================================
;
; RPC x64 Library
;
; Copyright (c) 2022 by fearless
;
; All Rights Reserved
;
; http://github.com/mrfearless
;
;
; This software is provided 'as-is', without any express or implied warranty. 
; In no event will the author be held liable for any damages arising from the 
; use of this software.
;
; Permission is granted to anyone to use this software for any non-commercial 
; program. If you use the library in an application, an acknowledgement in the
; application or documentation is appreciated but not required. 
;
; You are allowed to make modifications to the source code, but you must leave
; the original copyright notices intact and not misrepresent the origin of the
; software. It is not allowed to claim you wrote the original software. 
; Modified files must have a clear notice that the files are modified, and not
; in the original state. This includes the name of the person(s) who modified 
; the code. 
;
; If you want to distribute or redistribute any portion of this package, you 
; will need to include the full package in it's original state, including this
; license and all the copyrights.  
;
; While distributing this package (in it's original state) is allowed, it is 
; not allowed to charge anything for this. You may not sell or include the 
; package in any commercial package without having permission of the author. 
; Neither is it allowed to redistribute any of the package's components with 
; commercial applications.
;
;==============================================================================

;------------------------------------------------------------------------------
; RPC Prototypes
;------------------------------------------------------------------------------
RpcCheckConnection      PROTO :QWORD,:QWORD,:QWORD                              ; lpszHost, lpszPort, bSecure

RpcConnect              PROTO :QWORD,:QWORD,:QWORD,:QWORD,:QWORD                ; lpszHost, lpszPort, bSecure, lpszUserAgent, lpqwRpcHandle
RpcDisconnect           PROTO :QWORD                                            ; hRpc

RpcSetCookie            PROTO :QWORD,:QWORD                                     ; hRpc, lpszCookieData
RpcSetAuthBasic         PROTO :QWORD,:QWORD,:QWORD                              ; hRpc, lpszUsername, lpszPassword
RpcSetAcceptType        PROTO :QWORD,:QWORD,:QWORD                              ; hRpc, lpszAcceptType, qwAcceptType
RpcSetContentType       PROTO :QWORD,:QWORD,:QWORD                              ; hRpc, lpszContentType, qwContentType
RpcSetApiKey            PROTO :QWORD,:QWORD,:QWORD                              ; hRpc, lpszRpcApiKeyName, lpszRpcApiKeyValue

RpcSetPathVariable      PROTO :QWORD,:QWORD                                     ; hRpc, lpszVariable
RpcSetQueryParameters   PROTO :QWORD,:QWORD,:QWORD,:QWORD                       ; hRpc, lpszName, lpszValue, qwValue

RpcEndpointOpen         PROTO :QWORD,:QWORD,:QWORD,:QWORD,:QWORD                ; hRpc, lpszVerb, lpszEndpointUrl, lpSendData, qwSendDataSize
RpcEndpointClose        PROTO :QWORD                                            ; hRpc

RpcEndpointReadData     PROTO :QWORD,:QWORD,:QWORD                              ; hRpc, lpqwDataBuffer, lpqwTotalBytesRead
RpcEndpointFreeData     PROTO :QWORD                                            ; lpqwDataBuffer

RpcEndpointGET          PROTO :QWORD,:QWORD,:QWORD,:QWORD,:QWORD                ; hRpc, lpszEndpointUrl, lpqwData, lpqwDataSize, lpszDataToFile
RpcEndpointPOST         PROTO :QWORD,:QWORD,:QWORD,:QWORD,:QWORD,:QWORD,:QWORD  ; hRpc, lpszEndpointUrl, lpPostData, qwPostDataSize, lpqwData, lpqwDataSize, lpszDataToFile

RpcEndpointCall         PROTO :QWORD,:QWORD,:QWORD,:QWORD,:QWORD,:QWORD,:QWORD,:QWORD ; hRpc, qwRpcType, lpszEndpointUrl, lpSendData, qwSendDataSize, lpqwReceiveData, lpqwReceiveDataSize, lpszReceiveDataFile

RpcGetStatusCode        PROTO :QWORD                                            ; hRpc

;------------------------------------------------------------------------------
; RPC Constants
;------------------------------------------------------------------------------
.CONST
; RpcSetAcceptType qwAcceptType or RpcSetContentType qwContentType values (only common types):
RPC_MEDIATYPE_NONE      EQU 0 ;
RPC_MEDIATYPE_ALL       EQU 1 ; */*
RPC_MEDIATYPE_TEXT      EQU 2 ; text/*
RPC_MEDIATYPE_HTML      EQU 3 ; text/html
RPC_MEDIATYPE_XML       EQU 4 ; application/xml
RPC_MEDIATYPE_JSON      EQU 5 ; application/json
RPC_MEDIATYPE_FORM      EQU 6 ; application/x-www-form-urlencoded

; RpcEndpointCall qwRpcType:
RPC_GET                 EQU 0
RPC_POST                EQU 1
RPC_PUT                 EQU 2
RPC_PATCH               EQU 3
RPC_DELETE              EQU 4
RPC_HEAD                EQU 5
RPC_OPTIONS             EQU 6



;------------------------------------------------------------------------------
; RPC Example
;------------------------------------------------------------------------------
;
;    ;-------------------------------------------------------------------------
;    ; Connect to server at "http://localhost:1234"
;    ;-------------------------------------------------------------------------
;    Invoke RpcConnect, CTEXT("localhost"), CTEXT("1234"), FALSE, CTEXT("RPCAgent"), Addr hRpc
;
;    .IF rax == TRUE ; Successfully connected
;
;        Invoke RpcSetCookie, hRpc, Addr cookie ; persists for session, only need to call once if required
;
;        ;---------------------------------------------------------------------        
;        ; The following can be set per endpoint call depending on your requirements
;        ;---------------------------------------------------------------------
;        Invoke RpcSetAcceptType, hRpc, NULL, RPC_MEDIATYPE_JSON ; Accept json type data on return
;        Invoke RpcSetQueryParameters, hRpc, CTEXT("firstname"), CTEXT("bob"), 0 ; build query "?firstname=bob"
;        Invoke RpcSetQueryParameters, hRpc, CTEXT("lastname"), CTEXT("smith"), 0 ; add to query "&lastname=smith"
;
;        ;---------------------------------------------------------------------
;        ; Call endpoint with query: ("http://localhost:1234/acme/customer?firstname=bob&lastname=smith")
;        ;---------------------------------------------------------------------
;        Invoke RpcEndpointGET, hRpc, CTEXT("/acme/customer"), Addr CustomerData, Addr qwCustomerDataSize, NULL
;
;        .IF rax == TRUE
;
;            ;-----------------------------------------------------------------
;            ; RpcEndpointGET success - got customer data for bob smith
;            ; Parse returned data as JSON And do something with it...
;            ;-----------------------------------------------------------------
;            Invoke cJSON_Parse, CustomerData ; github.com/DaveGamble/cJSON
;            mov CustomerJson, rax
;
;            ;-----------------------------------------------------------------
;            ; Free endpoint data after it is no longer needed
;            ;-----------------------------------------------------------------
;            Invoke RpcEndpointFreeData, Addr CustomerData
;
;        .ELSE
;
;            ;-----------------------------------------------------------------
;            ; RpcEndpointGET failed - error retrieving data
;            ; Possibly show error message to user and disconnect and return
;            ;-----------------------------------------------------------------
;            Invoke RpcDisconnect, hRpc
;            ret
;
;        .ENDIF
;
;        ;---------------------------------------------------------------------
;        ; Note: http headers and query parameters are cleared after each call to an endpoint
;        ;---------------------------------------------------------------------
;        Invoke RpcSetAcceptType, hRpc, NULL, RPC_MEDIATYPE_JSON ; Accept json type data on return
;
;        ;---------------------------------------------------------------------
;        ; Call endpoint ("http://localhost:1234/acme/version")
;        ;---------------------------------------------------------------------
;        Invoke RpcEndpointGET, hRpc, CTEXT("/acme/version"), Addr VersionData, Addr qwVersionDataSize, NULL
;
;        .IF rax == TRUE
;
;            ;-----------------------------------------------------------------
;            ; RpcEndpointGET success - got version data 
;            ; Parse returned data as JSON and do something with it...
;            ;-----------------------------------------------------------------
;            Invoke cJSON_Parse, VersionData ; github.com/DaveGamble/cJSON
;            mov VersionJson, rax ; save pointer to parsed cJSON data
;            Invoke cJSON_GetObjectItemCaseSensitive, VersionJson, CTEXT("version") ; find "version" string
;            .IF rax != NULL
;                mov rbx, VersionJson
;                mov rax, [rbx].cJSON.valuestring ; Get value of "version" string
;                .IF rax != NULL
;                    Invoke lstrcpy, Addr szAcmeVersion, rax ; szAcmeVersion now contains "1.1.0"
;                .ENDIF  
;            .ENDIF    
;            Invoke cJSON_Delete, VersionJson ; free cJSON memory

;            ;-----------------------------------------------------------------
;            ; Free endpoint data after it is no longer needed
;            ;-----------------------------------------------------------------
;            Invoke RpcEndpointFreeData, Addr VersionData
;
;        .ELSE
;
;            ;-----------------------------------------------------------------
;            ; RpcEndpointGET failed - error retrieving data
;            ; Possibly show error message to user and disconnect and return
;            ;-----------------------------------------------------------------
;            Invoke RpcDisconnect, hRpc
;            ret
;
;        .ENDIF
;
;        ;---------------------------------------------------------------------
;        ; All processed successfully - disconnect and return
;        ;---------------------------------------------------------------------
;        Invoke RpcDisconnect, hRpc
;        ret
;
;    .ELSE
;
;        ;---------------------------------------------------------------------
;        ; Not connected - internet down, server down, wrong address and/or port etc
;        ; Possibly show error message to user and return
;        ;---------------------------------------------------------------------
;        ret
;
;    .ENDIF
;    ret



