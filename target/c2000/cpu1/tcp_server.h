#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include <stdint.h>

int32_t mainTcpServerOcpIf(uint8_t sn, uint8_t* buf, uint32_t size, uint16_t port);

#endif /* TCP_SERVER_H_ */
