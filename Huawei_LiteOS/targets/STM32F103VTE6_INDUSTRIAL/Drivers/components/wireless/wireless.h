#ifndef _HAL_WIFI_H_
#define _HAL_WIFI_H_

#include "string.h"
#include "stdio.h"
#include "dwt.h"
#include "los_task.ph"
#include "los_sem.h"
#include "atiny_socket.h"

typedef struct hal_wireless_module {
    void (*init)(void);
    int32_t (*cmd_and_reply)(const int8_t * buf, uint32_t len, char *reply1, char *reply2);
    int32_t (*send)(const int8_t * buf, uint32_t len);
    int32_t (*recv_timeout)(int8_t * buf, uint32_t len, uint32_t timeout);
    int32_t (*recv)(int8_t * buf, uint32_t len);
    int32_t (*connect)(const int8_t * host, const int8_t *port, int32_t proto);
    int32_t (*close)(void *p);
    int8_t * name;
}hal_wireless_modu;

void hal_wireless_init(void);
void hal_wireless_modu_register(hal_wireless_modu * m);
int32_t hal_wireless_cmd_and_reply(int8_t *buf, uint32_t len, char * reply1, char * reply2);
int32_t hal_wireless_send(const int8_t * buf, uint32_t len);
int32_t hal_wireless_recv_timeout(int8_t * buf, uint32_t len, uint32_t timeout);
int32_t hal_wireless_recv(int8_t * buf, uint32_t len);
int32_t hal_wireless_connect(const int8_t * host, const int8_t * port, int32_t proto);
int32_t hal_wireless_close(void * p);

void wireless_modu_register(void);


#endif /* _HAL_WIFI_H_ */
