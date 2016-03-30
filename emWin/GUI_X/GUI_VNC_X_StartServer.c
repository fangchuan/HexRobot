/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2012  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.16 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUI_VNC_X_StartServer.c
Purpose     : Starts the VNC server via TCP/IP.
              This code works with embOS/IP.
              However, it can be easily modified to work with
              different kernel and IP Stack
---------------------------END-OF-HEADER------------------------------
*/

#include <stdlib.h>

#include "IP.h"       // BSD socket interface
#include "RTOS.H"     // embOS header
#include "GUI.h"
#include "GUI_VNC.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static GUI_VNC_CONTEXT    _Context;
static struct sockaddr_in _Addr;

//
// embOS Stack area of the server
//
static OS_STACKPTR int _StackVNCServer[1000];

//
// embOS Task-control-block of the server
//
static OS_TASK         _VNCServer_TCB;

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _Send
*
* Function description
*   This function is called indirectly by the server; it's address is passed to the actual
*   server code as function pointer. It is needed because the server is independent
*   of the TCP/IP stack implementation, so details for the TCP/IP stack can be placed here.
*/
static int _Send(const U8 * buf, int len, void * pConnectionInfo) {
  int r;

  r = send((long)pConnectionInfo, (const char *)buf, len, 0);
  return r;
}

/*********************************************************************
*
*       _Recv
*
* Function description
*   This function is called indirectly by the server; it's address is passed to the actual
*   server code as function pointer. It is needed because the server is independent
*   of the TCP/IP stack implementation, so details for the TCP/IP stack can be placed here.
*/
static int _Recv(U8 * buf, int len, void * pConnectionInfo) {
  return recv((long)pConnectionInfo, (char *)buf, len, 0);
}

/*********************************************************************
*
*       _ListenAtTcpAddr
*
* Starts listening at the given TCP port.
*/
static int _ListenAtTcpAddr(U16 Port) {
  int sock;
  struct sockaddr_in addr;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(Port);
  addr.sin_addr.s_addr = INADDR_ANY;
  bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  listen(sock, 1);
  return sock;
}

/*********************************************************************
*
*       _ServerTask
*
* Function description
*   This routine is the actual server task.
*   It executes some one-time init code, then runs in an ednless loop.
*   It therefor does not terminate.
*   In the endless loop it
*     - Waits for a conection from a client
*     - Runs the server code
*     - Closes the connection
*/
static void _ServerTask(void) {
  int s, Sock, AddrLen;
  U16 Port;

  //
  // Prepare socket (one time setup)
  //
  Port = 5900 + _Context.ServerIndex; // Default port for VNC is is 590x, where x is the 0-based layer index
  //
  // Loop until we get a socket into listening state
  //
  do {
    s = _ListenAtTcpAddr(Port);
    if (s != -1) {
      break;
    }
    OS_Delay(100); // Try again
  } while (1);
  //
  // Loop once per client and create a thread for the actual server
  //
  while (1) {
    //
    // Wait for an incoming connection
    //
    AddrLen = sizeof(_Addr);
    if ((Sock = accept(s, (struct sockaddr*)&_Addr, &AddrLen)) == SOCKET_ERROR) {
      continue; // Error
    }
    //
    // Run the actual server
    //
    GUI_VNC_Process(&_Context, _Send, _Recv, (void *)Sock);
    //
    // Close the connection
    //
    closesocket(Sock);
    memset(&_Addr, 0, sizeof(struct sockaddr_in));
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_VNC_X_StartServer
*
* Function description
*   To start the server, the following steps are executed
*   - Make sure the TCP-IP stack is up and running
*   - Init the server context and attach it to the layer
*   - Start the thread (task) which runs the VNC server
* Notes:
*   (1) The first part of the code initializes the TCP/IP stack. In a typical
*       application, this is not required, since the stack should have already been
*       initialized some other place.
*       This could be done in a different module. (TCPIP_AssertInit() ?)
*/
int GUI_VNC_X_StartServer(int LayerIndex, int ServerIndex) {
  //
  // Init VNC context and attach to layer (so context is updated if the display-layer-contents change
  //
  GUI_VNC_AttachToLayer(&_Context, LayerIndex);
  _Context.ServerIndex = ServerIndex;
  //
  // Create task for VNC Server
  //
  OS_CREATETASK(&_VNCServer_TCB, "VNC Server", _ServerTask, 230, _StackVNCServer);
  //
  // O.k., server has been started
  //
  return 0;
}

/*********************************************************************
*
*       GUI_VNC_X_getpeername
*
* Function description
*   Retrieves the IP addr. of the currently connected VNC client.
*
*   Return values
*     IP addr. if VNC client connected
*     0 if no client connected
*/
void GUI_VNC_X_getpeername(U32 * Addr) {
  *Addr = _Addr.sin_addr.s_addr;
}

/*************************** End of file ****************************/

