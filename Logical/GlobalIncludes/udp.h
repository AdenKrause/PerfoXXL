/*
                             *******************
******************************* C HEADER FILE *******************************
**                           *******************                           **
**                                                                         **
** project  : X-Jet                                                        **
** filename : UDP.H                                                        **
** version  : 1.00                                                         **
** date     : 08.08.2008                                                   **
**                                                                         **
*****************************************************************************
** Abstract                                                                **
** ========                                                                **
** structs, variables and functions for udp interface to TiffBlaster       **
**                                                                         **
**                                                                         **
** Copyright (c) 2008, Krause-Biagosch GmbH                                **
** All rights reserved.                                                    **
**                                                                         **
*****************************************************************************
VERSION HISTORY:
----------------
Version     : 1.00
Date        : 08.08.2008
Revised by  : Herbert Aden
Description : Original version.

*/

#ifndef _UDP_H
#define _UDP_H

/****************************************************************************/
/**                                                                        **/
/**                           INCLUDE FILES                                **/
/**                                                                        **/
/****************************************************************************/
#include <string.h>
#include <asudp.h>
#include <AsBrStr.h>
#include <bur/plctypes.h>
#include <convert.h>
#include <standard.h>
#include "glob_var.h"
#include "in_out.h"

/****************************************************************************/
/**                                                                        **/
/**                       DEFINITIONS AND MACROS                           **/
/**                                                                        **/
/****************************************************************************/
#define SENDBUFSIZE (400)
#define RCVBUFSIZE (200)
#define DEBUGSTRSIZE (80)

#define INVALIDVALUE (999999999)
#define SEND_OK (1)
#define SEND_BUFFER_FULL (2)
#define SEND_INVALID_PLATETYPE1 (3)
#define SEND_INVALID_PLATETYPE2 (4)
#define SEND_NO_ID (5)
#define SEND_PLATE_NOT_LOADED (6)
#define SEND_MACHINE_NOT_STARTED (7)
#define SEND_WAIT (8)

#define MAXTOKENLENGTH 200
#define MAXTOKENS 20

/****************************************************************************/
/**                                                                        **/
/**                      TYPEDEFS AND STRUCTURES                           **/
/**                                                                        **/
/****************************************************************************/

/****************************************************************************/
/**                                                                        **/
/**                PROJECT GLOBAL VARIABLES                                **/
/**                                                                        **/
/****************************************************************************/
_GLOBAL STRING  RemoteIP[20];
_GLOBAL BOOL	saveRemoteIP;
_GLOBAL BOOL    gAbfrageOK,gAbfrageCancel;
_GLOBAL BOOL    gClearLinePressed;

/****************************************************************************/
/**                                                                        **/
/**                          EXPORTED FUNCTIONS                            **/
/**                                                                        **/
/****************************************************************************/
#ifndef _UDP_C
extern    void                init_UDP(void);
extern    void                cyclic_UDP(void);
#endif
/****************************************************************************/
/**                                                                        **/
/**                      EXPORTED VARIABLES                                **/
/**                                                                        **/
/****************************************************************************/
#ifndef _UDP_C
extern    SINT                buffer[SENDBUFSIZE];
extern    UINT                counter;

#else
/****************************************************************************/
/**                                                                        **/
/**                      TASK-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
_LOCAL    UdpOpen_typ         UDPopen_1,UDPopen_Rcv;
_LOCAL    UdpSend_typ         UDPsend_1;
_LOCAL    UdpRecv_typ         UDPreceive_1;
_LOCAL    UdpClose_typ        UDPclose_1;

_LOCAL    char                buffer[SENDBUFSIZE];
_LOCAL    UINT                counter;

_LOCAL    UINT                UDPStep;
_LOCAL    UINT                CounterMax;
_LOCAL    UINT                remotePort;
_LOCAL    char                receivebuffer[RCVBUFSIZE];
_LOCAL    UINT                UDPRcvStep;
_LOCAL    STRING              DebugRecStr[DEBUGSTRSIZE];
_LOCAL    STRING              DebugSendStr[DEBUGSTRSIZE];
_LOCAL    UINT                SendStatusInterval;

/****************************************************************************/
/**                                                                        **/
/**                      MODULE-LOCAL VARIABLES                            **/
/**                                                                        **/
/****************************************************************************/
static    TON_10ms_typ        StatusRequestTimer,StatusRequestTimeout;
static    BOOL                SendStatusRequest;
static    BOOL                SendJetStartOK,SendJetStopOK,
                              SendJetRefOK,SendJetControlvoltageOK;
static    USINT               SendJobAnswer,SendJobStates,SendTestAnswer;

static    R_TRIG_typ          MachineOnlineEdgeR,MachineOfflineEdgeR;
static    STRING              tmpID[MAXTOKENLENGTH];
#endif

/****************************************************************************/
/**                                                                        **/
/**                    MODULE-LOCAL VARIABLES                              **/
/**                                                                        **/
/****************************************************************************/
#ifdef _UDP_C
#endif

#endif
/****************************************************************************/
/**                                                                        **/
/**                                 EOF                                    **/
/**                                                                        **/
/****************************************************************************/


