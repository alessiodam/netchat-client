#ifndef LWIP_VETH_H
#define LWIP_VETH_H

#include <stdint.h>
#include <stdbool.h>
#include "lwip/arch.h"
#include "lwip/err.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"

extern struct netif veth_netif;

ssize_t veth_rx(void);

void veth_tx(u8_t *tx_buf, size_t size);

void *veth_receive(void);

err_t veth_transmit(struct netif *netif, struct pbuf *p);

err_t vethif_init(struct netif *netif);

#endif // LWIP_VETH_H
