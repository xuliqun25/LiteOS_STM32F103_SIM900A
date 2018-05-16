
#include "wireless.h"

static hal_wireless_modu *hal_wireless_m = NULL;

void hal_wireless_modu_register(hal_wireless_modu * m)
{
    if (NULL == hal_wireless_m && NULL != m)
        hal_wireless_m = m;
}

int32_t hal_wireless_cmd_and_reply(int8_t *buf, uint32_t len, char * reply1, char * reply2)
{
    if (hal_wireless_m && hal_wireless_m->cmd_and_reply){
        return hal_wireless_m->cmd_and_reply(buf, len, reply1, reply2);
    }
    return -1;
}

int32_t hal_wireless_send(const int8_t * buf, uint32_t len)
{
    if (hal_wireless_m && hal_wireless_m->send){
        return hal_wireless_m->send(buf, len);
    }
    return -1;
}

void hal_wireless_init()
{
    if(hal_wireless_m && hal_wireless_m->init){
        hal_wireless_m->init();
    }
}

int32_t hal_wireless_recv_timeout(int8_t * buf, uint32_t len, uint32_t timeout)
{
    if (hal_wireless_m && hal_wireless_m->recv){
        return hal_wireless_m->recv_timeout(buf, len, timeout);
    }
    return -1;
}

int32_t hal_wireless_recv(int8_t * buf, uint32_t len)
{
    if (hal_wireless_m && hal_wireless_m->recv){
        return hal_wireless_m->recv(buf, len);
    }
    return -1;
}

int32_t hal_wireless_connect(const int8_t * host, const int8_t * port, int32_t proto)
{
    if(hal_wireless_m && hal_wireless_m->connect){
        return hal_wireless_m->connect(host, port, proto);
    }
    return -1;
}

int32_t hal_wireless_close(void * p)
{
    if (hal_wireless_m && hal_wireless_m->close){
        return hal_wireless_m->close(p);
    }
    return -1;
}
