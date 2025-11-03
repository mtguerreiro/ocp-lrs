//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include <string.h>
#include "ocp/ocpIf.h"
#include "wiznet/socket.h"
#include "wiznet/wizchip_conf.h"
#include "tcp_server.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t mainTcpServerOcpIf(uint8_t sn, uint8_t* buf, uint32_t size, uint16_t port){
    
   int32_t ret;
   uint8_t *p;
   uint32_t rxSize;
   uint32_t txSize;

#if TCP_SERVER_ECHO_CFG_DEBUG == 1
   uint8_t destip[4];
   uint16_t destport;
#endif

   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(sn) & Sn_IR_CON)
         {
#if TCP_SERVER_ECHO_CFG_DEBUG == 1
            getSn_DIPR(sn, destip);
            destport = getSn_DPORT(sn);

            printf("%s (sn %d): Connected - %d.%d.%d.%d : %d\r\n",
                   __func__, sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
            setSn_IR(sn,Sn_IR_CON);
         }
         if((getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't occur
         {
            ret = recv(sn, buf, 4);
            if(ret != 4)  return -1;

            memcpy((void *)&rxSize, (void *)buf, sizeof(rxSize));

            ret = recv(sn, buf, rxSize);
            if(ret != rxSize) return -1;

            p = buf;
            ret = ocpIf(buf, rxSize >> 1, (void **)&p, size);
            txSize = ret << 1;

            ret = send(sn, (uint8_t *)&txSize, 4);
            if(ret < 4){
                close(sn);
                return -1;
            }

            ret = send(sn, p, txSize);
            if(ret != txSize){
                close(sn);
                return -1;                
            }
         }
         break;
      case SOCK_CLOSE_WAIT :
#if TCP_SERVER_ECHO_CFG_DEBUG == 1
         printf("%s (sn %d): CloseWait\r\n", __func__, sn);
#endif
         if((ret = disconnect(sn)) != SOCK_OK) return ret;
#if TCP_SERVER_ECHO_CFG_DEBUG == 1
         printf("%s (sn %d): Socket closed\r\n", __func__, sn);
#endif
         break;
      case SOCK_INIT :
#if TCP_SERVER_ECHO_CFG_DEBUG == 1
         printf("%s (sn %d): Listen, TCP server loopback, port [%d]\r\n",
                __func__, sn, port);
#endif
         if((ret = listen(sn)) != SOCK_OK) return ret;
         break;
      case SOCK_CLOSED:
#if TCP_SERVER_ECHO_CFG_DEBUG == 1
         printf("%s (sn %d): TCP server loopback start\r\n", __func__, sn);
#endif
         if((ret = socket(sn, Sn_MR_TCP, port, SF_TCP_NODELAY)) != sn) return ret;
#if TCP_SERVER_ECHO_CFG_DEBUG == 1
         printf("%s (sn %d): Socket opened, port [%d]\r\n", __func__, sn, port);
#endif
         break;
      default:
         break;
   }
   
   return 1;
}
//-----------------------------------------------------------------------------
//=============================================================================
