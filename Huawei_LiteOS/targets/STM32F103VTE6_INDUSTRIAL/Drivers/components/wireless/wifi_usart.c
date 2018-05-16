/*  use usart 3 for serial-wifi-module 

    usart3: tx pb10
                rx pb11

*/


#include "netconf.h"
#if NETWORK_TYPE == ESP8266_USART_WIFI
#include "wireless.h"
#include "wifi_usart.h"

#define      MODULE_NAME                          "ESP8266"
#define      ESP8266_ApSsid                       "TP-LINK_80D8BE"     //要连接的热点的名称
#define      ESP8266_ApPwd                        "87654321"           //要连接的热点的密钥
#define      MAX_USART_BUF_LEN                    512

void wifi_usart_init(void);
int32_t wifi_usart_cmd(const int8_t * cmd, uint32_t len, char * reply1, char * reply2);
int32_t wifi_usart_send(const int8_t * cmd, uint32_t len);
int32_t  wifi_usart_recv_timeout(int8_t * buf, uint32_t len, uint32_t timeout);
int32_t  wifi_usart_recv(int8_t * buf, uint32_t len);
int32_t wifi_usart_connect(const int8_t * host, const int8_t * port, int32_t proto);
int32_t wifi_usart_close(void * p);

static hal_wireless_modu wifi_modu = {
    .init = wifi_usart_init,
    .cmd_and_reply = wifi_usart_cmd,
    .send = wifi_usart_send,
    .recv_timeout = wifi_usart_recv_timeout,
    .recv = wifi_usart_recv,
    .connect = wifi_usart_connect,
    .close = wifi_usart_close,
    .name = MODULE_NAME,
};
static uint32_t wifi_usart_irq_sem;
static uint32_t wifi_recv_sem;
static uint32_t wifi_resp_sem;

uint32_t g_wifiTskHandle;

uint8_t usart_recv_buf[MAX_USART_BUF_LEN] = {0};
uint8_t resp_buf[MAX_USART_BUF_LEN] = {0};
uint8_t data_buf[MAX_USART_BUF_LEN] = {0};

uint32_t get_data_len = 0;

uint32_t wi = 0;
uint32_t ri = 0;

void WIFI_IRQHandler(void)
{    
	uint8_t __clear;

    __clear = __clear;
    if(USART_GetITStatus(WIFI_USART, USART_IT_RXNE) != RESET)
    {
		usart_recv_buf[wi++] = USART_ReceiveData(WIFI_USART);
		if (wi >=MAX_USART_BUF_LEN)wi = 0; 
    }
    else if (USART_GetITStatus(WIFI_USART, USART_IT_IDLE) != RESET)
    {
		__clear = USART3->SR;
        __clear = USART3->DR;
        LOS_SemPost(wifi_usart_irq_sem);
    }
}


int  read_resp(uint8_t * buf)
{
    uint32_t len = 0;
    if (NULL == buf){
        return -1;  
    }

    if (wi == ri){
        return 0;
	}

    if (wi > ri){
        len = wi - ri;
        memcpy(buf, &usart_recv_buf[ri], len);
    } else {
        uint32_t tmp_len = MAX_USART_BUF_LEN - ri;
        memcpy(buf, &usart_recv_buf[ri], tmp_len);
        memcpy(buf + tmp_len, usart_recv_buf, wi);
        len = wi + tmp_len;
    }    
    ri = wi;
    return len;
}

void Wifi_USART_Config()
{

    //enable clock of pb, usart3
    RCC_AHB1PeriphClockCmd(WIFI_USART_TX_GPIO_CLK | WIFI_USART_RX_GPIO_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(WIFI_USART_CLK, ENABLE);

    //set gpio af
    GPIO_PinAFConfig(WIFI_USART_TX_GPIO_PORT, WIFI_USART_TX_PIN_SOURCE, WIFI_USART_TX_AF);
    GPIO_PinAFConfig(WIFI_USART_RX_GPIO_PORT, WIFI_USART_RX_PIN_SOURCE, WIFI_USART_RX_AF);

    //set gpio mode, pp ,pull-up
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStruct.GPIO_Pin = WIFI_USART_RX_GPIO_PIN;
    GPIO_Init(WIFI_USART_RX_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = WIFI_USART_TX_GPIO_PIN;
    GPIO_Init(WIFI_USART_TX_GPIO_PORT, &GPIO_InitStruct);

    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = WIFI_USART_BUARDRATE;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(WIFI_USART, &USART_InitStruct);
    
	USART_ClearFlag(WIFI_USART, USART_FLAG_TC);
    USART_Cmd(WIFI_USART, ENABLE);

    USART_ITConfig(WIFI_USART, USART_IT_RXNE, ENABLE);    
    USART_ITConfig(WIFI_USART, USART_IT_IDLE, ENABLE);


    LOS_HwiCreate(WIFI_IRQn, 0, 0,WIFI_IRQHandler, NULL);//songjn add
}

uint32_t wait_for_resp(char * reply1, char *reply2)
{
    uint32_t ret = 0;
    char * pp1, *pp2;
    uint32_t retry_t;

    //big num loop for catch "ready" in reboot log, simple will get reply in 3-5 loops
    for(retry_t = 0; retry_t < 50; retry_t++){ 

//        WIFI_LOG("get resp %s", resp_buf);
        
        LOS_SemPend(wifi_resp_sem, osDelay(osMs2Tick(100)));


        if (NULL != reply1 && NULL != reply2){
            pp1 = strstr((const char *)resp_buf, (const char *)reply1);
            pp2 = strstr((const char *)resp_buf, (const char *)reply2);
            ret = (NULL != pp1 || NULL != pp2);
        } else if (NULL != reply1) {
            pp1 = strstr((const char *)resp_buf, (const char *)reply1);
            ret = (NULL != pp1);
        } else if (NULL != reply2) {
            pp2 = strstr((const char *)resp_buf, (const char *)reply2);
            ret = (NULL != pp2);
        } else {
            ret = 1; // no check reply
        } 
        
        if (ret){
            memset(resp_buf, 0, MAX_USART_BUF_LEN);
            return ret;
        }
    }
		return ret;

}
int32_t  wifi_usart_recv(int8_t * buf, uint32_t len)
{
    int tmp = 0;
	
    LOS_SemPend(wifi_recv_sem, LOS_WAIT_FOREVER);

    tmp = get_data_len;
    if (tmp){
        memcpy(buf, data_buf, tmp);
		get_data_len = 0;
    }
    return tmp;
}
int32_t  wifi_usart_recv_timeout(int8_t * buf, uint32_t len, uint32_t timeout)
{
    int tmp = 0;
	
    LOS_SemPend(wifi_recv_sem,osMs2Tick(timeout));

    tmp = get_data_len;
    if (tmp){
        memcpy(buf, data_buf, tmp);
		get_data_len = 0;
    }
    return tmp;
}
int32_t wifi_usart_cmd(const int8_t * cmd, uint32_t len, char * reply1, char * reply2)
{
    uint32_t i, ret = 0;

    for (i = 0; i < len ; i++){
        USART_SendData(WIFI_USART,cmd[i]);
		while (USART_GetFlagStatus(WIFI_USART, USART_FLAG_TXE) == RESET);
    }
    if (wait_for_resp(reply1, reply2)){
        ret = len; //get excepted reply, return send len
    } 
	return ret;
}

int32_t wifi_usart_send(const int8_t * buf, uint32_t len)
{
    uint32_t ret = 0;

    char * cmd = "AT+CIPSEND=%d\r\n";
    char tmp_cmd[64] = {0};

    snprintf(tmp_cmd, 64, cmd, len);

    printf("kiwi: start send:%s\n", tmp_cmd);
    wifi_usart_cmd((const int8_t *)tmp_cmd, strlen(tmp_cmd), ">", NULL);
    
    ret = wifi_usart_cmd((const int8_t*)buf, len, "SEND OK", NULL);
	return ret;
}
int32_t wifi_usart_connect(const int8_t * host, const int8_t * port, int32_t proto)
{
    uint8_t tmp_cmd[64] = {0};
    const char * cmd_fmt = "AT+CIPSTART=\"%s\",\"%s\",%s\r\n";

    snprintf((char*)tmp_cmd, 64, cmd_fmt, proto == ATINY_PROTO_UDP? "UDP" : "TCP", host, port);
    wifi_usart_cmd((const int8_t *)tmp_cmd, strlen((char*)tmp_cmd), "OK", "ALREADY CONNECTED");
    return 0;
}
int32_t wifi_usart_close(void * p)
{
    char * cmd = "AT+CIPCLOSE\r\n";

    printf("kiwi: close connect\n");
    wifi_usart_cmd((const int8_t *)cmd, strlen(cmd), "OK", NULL);
		return 0;
}
void store_resp_buf(uint8_t * buf, uint32_t len)
{
    strcat((char*)resp_buf, (const char *)buf);
}

void task_wifi()
{
    uint32_t recv_len = 0;
    uint8_t tmp[512] = {0};
    int ret = 0;
    
    ret = LOS_SemCreate(1, &wifi_usart_irq_sem);
    if (ret != LOS_OK)
    {
        WIFI_LOG("init wifi_recv_sem failed!");
        return;
    }
    ret = LOS_SemCreate(1, &wifi_recv_sem);
    if (ret != LOS_OK)
    {
        WIFI_LOG("init wifi_recv_sem failed!");
        
        LOS_SemDelete(wifi_usart_irq_sem);
        return;
    }

    ret = LOS_SemCreate(1, &wifi_resp_sem);
    if (ret != LOS_OK)
    {
        WIFI_LOG("init wifi_recv_sem failed!");
        LOS_SemDelete(wifi_recv_sem);
        LOS_SemDelete(wifi_usart_irq_sem);
        return;
    }
    

    while(1){
        LOS_SemPend(wifi_usart_irq_sem, LOS_WAIT_FOREVER);
        memset(tmp, 0, sizeof(tmp));
        recv_len = read_resp(tmp);
//        WIFI_LOG("get recv_len len = %d:%s", recv_len, tmp);

        if (strstr((const char *)tmp, "+IPD")){
//            WIFI_LOG("get +IPD data");
            uint8_t * ch = (uint8_t*)strstr((const char *)tmp, ",");
            uint32_t data_len = 0;
            if (NULL != ch)
            {
                for (ch++; *ch <= '9' && *ch >= '0' ;ch++){
                    data_len = (data_len * 10 + (*ch - '0'));
                }

            get_data_len = data_len;
            ch = (uint8_t*)strstr((const char *)tmp, ":");
            memcpy(data_buf, ch + 1, data_len);
            
            LOS_SemPost(wifi_recv_sem);
            }
            
        }
        else {
            store_resp_buf(tmp, recv_len);
            LOS_SemPost(wifi_resp_sem);
            
        }
    }        
}

uint32_t create_usart_recv_thread()
{
    uint32_t uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 0;
    task_init_param.pcName = "task_usart_wifi_recv";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)task_wifi;
    task_init_param.uwStackSize = 0x1000;

    uwRet = LOS_TaskCreate(&g_wifiTskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;

}

uint8_t ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
	switch ( enumMode )
	{
		case STA:
			return wifi_usart_cmd ( "AT+CWMODE_CUR=1\r\n", strlen("AT+CWMODE_CUR=1\r\n"), "OK", "no change" ); 
		
	  case AP:
		  return wifi_usart_cmd ( "AT+CWMODE_CUR=2\r\n", strlen("AT+CWMODE_CUR=2\r\n"),"OK", "no change" ); 
		
		case STA_AP:
		  return wifi_usart_cmd ( "AT+CWMODE_CUR=3\r\n", strlen("AT+CWMODE_CUR=3\r\n"),"OK", "no change" ); 
		
	  default:
		  return 0;
  }
	
}

uint8_t ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", pSSID, pPassWord );
	
	return wifi_usart_cmd ( (int8_t*)cCmd, strlen(cCmd), "OK", NULL );
	
}

uint8_t ESP8266_RST ( )
{
	char *cCmd = "AT+RST\r\n";
	
	return wifi_usart_cmd ( (int8_t*)cCmd, strlen(cCmd), "ready", NULL );
	
}

void wifi_usart_init()
{
    //1.
    
    WIFI_LOG("Config ESP8266......\n");
    Wifi_USART_Config();
    create_usart_recv_thread();

    ESP8266_Net_Mode_Choose(STA);

    ESP8266_JoinAP ( ESP8266_ApSsid, ESP8266_ApPwd );


    WIFI_LOG("connect wifi now!!\n");
    

}

void wireless_modu_register(void)
{
    hal_wireless_modu_register(&wifi_modu);
    WIFI_LOG("register wireless module(%s) complete.", wifi_modu.name);
}

#endif /* #if NETWORK_TYPE == ESP8266_USART_WIFI */
