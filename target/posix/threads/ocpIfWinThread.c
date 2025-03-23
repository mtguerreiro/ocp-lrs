
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpIfThread.h"

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "winsock2.h"

/* Open controller project */
#include "ocp/ocpIf.h"

//============================================================================


//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void *ocpIfThreadProcess(void *param);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

#define OCP_IF_THREAD_BACKLOG       5
#define OCP_IF_THREAD_BUFFER_SIZE   1024

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static WSADATA wsaData;
static SOCKET server_socket = INVALID_SOCKET, client_socket = INVALID_SOCKET;
static struct sockaddr_in server_addr, client_addr;
static int client_len;

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void *ocpIfThread(void *ptr){

    pthread_t ocpIfThreadProcessHandle;

    int status;

    /* Initializes Winsock */
    if( WSAStartup(MAKEWORD(2,2),&wsaData) != 0 ){
        printf("WSAStartup failed. Error Code : %d",WSAGetLastError());
        fflush( stdout );
        return 0;
    }

    /* Creates the server socket */
    if( (server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET ){
        printf("%s: Error creating socket. Error Code : %d", __FUNCTION__, WSAGetLastError());
        fflush( stdout );
        WSACleanup();
        return 0;
    }

    /* Sets up the server address structure */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(OCP_IF_THREAD_SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    /* Binds the server socket to the address */
    status = bind( server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr) );
    if( status < 0 ){
        printf("%s: Error binding socket. bind returned: %d\n\r", __FUNCTION__, status);
        return 0;
    }

    /* Listens for incoming connections */
    status = listen(server_socket, OCP_IF_THREAD_BACKLOG); 
    if( status < 0 ){
        printf("%s: Error listening for connections. listen returned: %d\n\r", __FUNCTION__, status);
        close(server_socket);
        return 0;
    }

    printf("%s: OCP interface ready and running.\n\r", __FUNCTION__);

    while(1){
        /* Waits for a connection */
        client_len = sizeof(client_addr);
        client_socket = accept( server_socket , (struct sockaddr *)&client_addr, &client_len );
        if( client_socket < 0 ) {
            printf("%s: Error accepting connection. accept returned: %d\n\r", __FUNCTION__, client_socket);
            close(server_socket);
            return 0;
        }

        printf("%s: Client connected from %s\n", __FUNCTION__, inet_ntoa(client_addr.sin_addr));

        pthread_create( &ocpIfThreadProcessHandle, NULL, ocpIfThreadProcess, (void *)&client_socket);
        pthread_join( ocpIfThreadProcessHandle, NULL);

    }

    close(server_socket);

    printf("Thread ocp\n\r");

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void *ocpIfThreadProcess(void *param){

    int sd = *( (int *)param );
    char recvbuf[OCP_IF_THREAD_BUFFER_SIZE];
    int32_t n;
    int32_t nrx;
    int32_t ret;
    int32_t size;

    int32_t status;

    uint8_t *p;

    while(1){
        /*
         * After receiving a connection, reads the first 4 bytes. They should
         * represent the number of bytes to come next.
         */
        nrx = 0;
        while( nrx < 4 ){
          n = recv(sd, (void *)&recvbuf[nrx], 4 - nrx, 0);
          if( n <= 0 ){
              printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
              fflush(stdout);
              break;
          }
          nrx += n;
        }
        if( nrx != 4 ){
          printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
          fflush(stdout);
          break;
        }
        size = *((uint32_t *)recvbuf);

        /* Read the number of expected bytes */
        nrx = 0;
        while( nrx < size ){
          n = recv(sd, (void *)&recvbuf[nrx], size - nrx, 0);
          if( n <= 0 ){
              printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
              fflush(stdout);
              break;
          }
          nrx += n;
        }
        if( nrx != size ){
          printf("%s: error receiving all expected data from socket %d, closing socket\r\n", __FUNCTION__, sd);
          fflush(stdout);
          break;
        }

        /* Calls the interface */
        p = (uint8_t *)( recvbuf );
        ret = ocpIf((void *)p, size, (void **)(&p), OCP_IF_THREAD_BUFFER_SIZE);

        /*
         * Now, sends the reply. The reply consists of the command status/size
         * (4 bytes), followed by data (if any).
         */
        // TODO: should we also do a while loop to send the data? Like in receiving?

        /* Writes back the command status */
        //*( (int32_t *)recvbuf ) = ret;
        n = send(sd, (void *)&ret, 4, 0);
        if( n < 4 ){
            printf("%s: error responding to client\'s request\r\n", __FUNCTION__);
            fflush(stdout);
            break;
        }

        /* Writes data */
        if( ret > 0 ){
            n = send(sd, (void *)p, ret, 0);
            if( n < ret ){
                printf("%s: error responding to client\'s request\r\n", __FUNCTION__);
                fflush(stdout);
            }
        }
        break;
    }

    /* Closes connection */
    status = shutdown(sd, 0);
    if( status < 0 ){
        printf("%s: Shutdown failed. shutdown returned: %d\n", __FUNCTION__, status);
    }

    close(sd);

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
