/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  ：usart2.c
 * 描述    ：将printf函数重定向到USART2。这样就可以用printf函数将单片机的数据
 *           打印到PC上的超级终端或串口调试助手。         
 * 实验平台：野火STM32开发板
 * 硬件连接：---------------------
 *          | 									  |
 *          | PA2  - USART2(Tx)   |
 *          | PA3  - USART2(Rx)   |
 *          |                     |
 *           ---------------------
 * 库版本  ：ST3.5.0
 *
 * 作者    ：wildfire team 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/

#include "sim_usart.h"
#include <stdarg.h>
#include "los_task.ph"
#include "los_sem.h"
#include "sys_init.h"
#include "wireless.h"
#include "los_queue.h"
#include "los_mux.h"
#include "atiny_adapter.h"
#if NETWORK_TYPE == SIM_900A

#define     MODULE_NAME             "SIM_900A"
#define      MAX_USART_BUF_LEN     1024
static UINT32 puwQueueID;
static UINT32 g_recv_mux;
static UINT32 sim_usart_irq_sem;
static UINT32 sim_resp_sem;
UINT32 g_simTskHandle;
UART_HandleTypeDef sim_uart_handle;

uint8_t usart_recv_buf[MAX_USART_BUF_LEN] = {0};
uint8_t resp_buf[MAX_USART_BUF_LEN] = {0};

void bsp_USART2_IRQHandler(void);
void sim_usart_init(void);
int32_t sim_usart_cmd(const int8_t * cmd, uint32_t len, char * reply1, char * reply2);
int32_t sim_usart_send(const int8_t * cmd, uint32_t len);
int32_t sim_usart_recv(int8_t * buf, uint32_t len);
int32_t sim_usart_recv_timeout(int8_t * buf, uint32_t len, uint32_t timeout);
int32_t sim_usart_connect(const int8_t * host, const int8_t * port, int32_t proto);
int32_t sim_usart_close(void * p);

static hal_wireless_modu sim_modu = {
    .init = sim_usart_init,
    .cmd_and_reply = sim_usart_cmd,
    .send = sim_usart_send,
    .recv_timeout = sim_usart_recv_timeout,
    .recv = sim_usart_recv,
    .connect = sim_usart_connect,
    .close = sim_usart_close,
    .name = MODULE_NAME,
};


/*
 * 函数名：USART2_Config
 * 描述  ：USART2 GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void SIM_USART_Config(void)
{
    /* USART2 mode config */
    sim_uart_handle.Instance = SIM_USART;
    sim_uart_handle.Init.BaudRate = 115200;
    sim_uart_handle.Init.WordLength = UART_WORDLENGTH_8B;
    sim_uart_handle.Init.StopBits = UART_STOPBITS_1;
    sim_uart_handle.Init.Parity = UART_PARITY_NONE;
    sim_uart_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    sim_uart_handle.Init.Mode = UART_MODE_RX | UART_MODE_TX;

    if(HAL_UART_Init(&sim_uart_handle) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_UART_CLEAR_FLAG(&sim_uart_handle, UART_FLAG_TC);
    LOS_HwiCreate(USART2_IRQn, 1,0,bsp_USART2_IRQHandler,NULL);
    __HAL_UART_ENABLE_IT(&sim_uart_handle, UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&sim_uart_handle, UART_IT_IDLE);
}


//中断缓存串口数据
uint32_t wi = 0;
uint32_t ri = 0;

typedef struct {
    uint32_t len;
    uint8_t *addr;
}QUEUE_BUFF;
void bsp_USART2_IRQHandler(void)
{
     uint8_t __clear;

     if(__HAL_UART_GET_FLAG(&sim_uart_handle, UART_FLAG_RXNE) != RESET)
     {
        LOS_MuxPend(g_recv_mux, osDelay(osMs2Tick(50)));
        usart_recv_buf[wi++] = (uint8_t)(sim_uart_handle.Instance->DR & (uint8_t)0x00FF);
        if (wi >= MAX_USART_BUF_LEN) wi = 0;
        LOS_MuxPost(  g_recv_mux);
     }
     else if(__HAL_UART_GET_FLAG(&sim_uart_handle,UART_FLAG_IDLE) != RESET)
     {
        __clear = SIM_USART->SR;
        __clear = SIM_USART->DR;
        (void)__clear;
        LOS_SemPost(sim_usart_irq_sem);
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

/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
uint32_t wait_for_resp(char * reply1, char *reply2)
{
    uint32_t ret = 0;
    char * pp1, *pp2;
    uint32_t retry_t;
    
    // got 10 resp for reply
    for(retry_t = 0; retry_t < 10; retry_t++){ 
        LOS_SemPend(sim_resp_sem, LOS_WAIT_FOREVER);
        printf("get resp :%s ", resp_buf);

        if (NULL != reply1 && NULL != reply2){
            pp1 = strstr((const char *)resp_buf, reply1);
            pp2 = strstr((const char *)resp_buf, reply2);
            ret = (NULL != pp1 || NULL != pp2);
        } else if (NULL != reply1) {
            pp1 = strstr((const char *)resp_buf, reply1);
            ret = (NULL != pp1);
        } else if (NULL != reply2) {
            pp2 = strstr((const char *)resp_buf, reply2);
            ret = (NULL != pp2);
        }

        if (ret){
            memset(resp_buf, 0, MAX_USART_BUF_LEN);
            return ret;
        }
    }
    return ret;
}

int32_t sim_usart_cmd(const int8_t * cmd, uint32_t len, char * reply1, char * reply2)
{
    uint32_t i, ret = 0;
    for (i = 0; i < len ; i++){
        //USART_SendData(SIM_USART,cmd[i]);
        sim_uart_handle.Instance->DR = (cmd[i] & (uint16_t)0x00FF);
        while (__HAL_UART_GET_FLAG(&sim_uart_handle, UART_FLAG_TXE) == RESET);
    }
    if (wait_for_resp(reply1, reply2)){
        ret = len; //get excepted reply, return send len
    }
	return ret;
}

void store_resp_buf(uint8_t * buf, uint32_t len)
{
    if (NULL == buf || 0 == len || len > MAX_USART_BUF_LEN)
        return;

    //clear \0 in buf
    int i, j;
    for (i = 0, j = 0; i < len; i++)
    {
        if (buf[i]) buf[j++] = buf[i];
    }

    if ((len + strlen((char*)resp_buf)) > MAX_USART_BUF_LEN)
    {
        memset(resp_buf, 0, MAX_USART_BUF_LEN);
        return;
    }
    
    strncat((char*)resp_buf, (const char *)buf, j);
}
int32_t sim_usart_connect(const int8_t * host, const int8_t * port, int proto)
{
    char tmp_cmd[64] = {0};
    sim_usart_cmd((int8_t*)"AT+CIPCLOSE=1\r",strlen("AT+CIPCLOSE=1\r"),"CLOSE OK","ERROR");
    sim_usart_cmd((int8_t*)"AT+CIPSHUT\r",strlen("AT+CIPSHUT\r"),"SHUT OK", NULL);
    sim_usart_cmd((int8_t*)"AT+CGCLASS=\"B\"\r",strlen("AT+CGCLASS=\"B\"\r"),"OK", NULL);
    sim_usart_cmd((int8_t*)"AT+CGDCONT=1,\"IP\",\"CMNET\"\r",strlen("AT+CGDCONT=1,\"IP\",\"CMNET\"\r"),"OK",NULL);
    sim_usart_cmd((int8_t*)"AT+CGATT=1\r",strlen("AT+CGATT=1\r"),"OK",NULL);
    sprintf(tmp_cmd,"AT+CIPSTART=\"%s\",\"%s\",\"%s\"\r", proto == ATINY_PROTO_UDP? "UDP" : "TCP", host, port);
    if(0 == sim_usart_cmd((int8_t*)tmp_cmd,strlen(tmp_cmd),NULL,"CONNECT OK")) return -1;
    sim_usart_cmd((int8_t*)"AT+CIPQSEND=1\r",strlen("AT+CIPQSEND=1\r"),"OK",NULL);
    sim_usart_cmd((int8_t*)"AT+CIPHEAD=1\r",strlen("AT+CIPHEAD=1\r"),"OK",NULL);
	return 0;
}

int32_t  sim_usart_recv(int8_t * buf, uint32_t len)
{
    UINT32 qlen = sizeof(QUEUE_BUFF);
	
    QUEUE_BUFF  qbuf = {0, NULL};
    int ret = LOS_QueueReadCopy(puwQueueID,&qbuf, &qlen, LOS_WAIT_FOREVER);
    printf("get ret:%x len %d, addr:%p\n", ret, qbuf.len, qbuf.addr);

    if (qbuf.len){
        memcpy(buf, qbuf.addr, qbuf.len);
        atiny_free(qbuf.addr);
    }
    return qbuf.len;
}

int32_t  sim_usart_recv_timeout(int8_t * buf, uint32_t len, uint32_t timeout)
{
    UINT32 qlen = sizeof(QUEUE_BUFF);
	
    QUEUE_BUFF  qbuf = {0, NULL};
    int ret = LOS_QueueReadCopy(puwQueueID,&qbuf, &qlen, osMs2Tick(timeout));
    printf("get ret:%x len %d, addr:%p\n", ret, qbuf.len, qbuf.addr);

    if (qbuf.len){
        memcpy(buf, qbuf.addr, qbuf.len);
        atiny_free(qbuf.addr);
    }
    return qbuf.len;
}
int32_t sim_usart_send(const int8_t * buf, uint32_t len)
{
    char tmp_cmd[64] = {0};
    sprintf(tmp_cmd, "AT+CIPSEND=%d\r",(uint16_t)len);
    if(sim_usart_cmd((int8_t*)tmp_cmd, strlen(tmp_cmd), ">", NULL))
    {
        if(sim_usart_cmd((int8_t *)buf, (uint32_t)len, "DATA ACCEPT", NULL))
            return len;
    }
    return -1;
}
int sim_usart_close(void * p)
{
    char * cmd = "AT+CIPCLOSE\r\n";
    printf("sim: close connect\n");
    sim_usart_cmd((int8_t*)cmd, strlen(cmd), "OK", NULL);
    return 0;
}

void task_sim()
{
    int ret = 0;
    uint32_t recv_len = 0;
    uint8_t tmp[MAX_USART_BUF_LEN] = {0};
    int i = 0;
    QUEUE_BUFF  buf;
    
    ret = LOS_SemCreate(0, &sim_usart_irq_sem);
    if (ret != LOS_OK)
    {
        SIM_LOG("init sim_recv_sem failed!");
        goto usart_irq_sem_init_failed;
    }
    ret = LOS_SemCreate(0, &sim_resp_sem);
    if (ret != LOS_OK)
    {
        SIM_LOG("init sim_recv_sem failed!");
        goto resp_sem_init_failed;
    }
    ret = LOS_QueueCreate("dataQueue", 16, &puwQueueID, 0, sizeof(QUEUE_BUFF));
    if (ret != LOS_OK)
    {
        printf("init puwQueueID failed!");
        goto queue_init_failed;
    }
    
    ret = LOS_MuxCreate(&g_recv_mux);
    if (ret != LOS_OK)
    {
        printf("init g_recv_mux failed!");
        goto mutex_init_failed;
    }
    
    while(1){
        LOS_SemPend(sim_usart_irq_sem, LOS_WAIT_FOREVER);
        memset(tmp, 0, sizeof(tmp));
        LOS_MuxPend(g_recv_mux, osDelay(osMs2Tick(50)));
        recv_len = read_resp(tmp);
        LOS_MuxPost(g_recv_mux);
        
        if (0 >= recv_len)
            continue;

        uint32_t data_len = 0;
        char * p1, * p2;
        SIM_LOG("get recv_len len = %d:%s\n", recv_len, tmp);
        p1 = (char *)tmp;
        for (;;){
            if (0 == memcmp(p1, "\r\n+IPD", strlen("\r\n+IPD")))
            {
                //data format "+IPD,len:xxxxx"
                p2 = strstr(p1, ",");
                if (NULL == p2)
                {
                    printf("got +IPD format invaild!");
                    p1 += 4;
                    continue;
                }
                
                for (p2++; *p2 <= '9' && *p2 >= '0' ;p2++)
                {
                    data_len = (data_len * 10 + (*p2 - '0'));
                }
                p2++; //over ':'

                buf.len = data_len;
                buf.addr = atiny_malloc(data_len);
                if (NULL == buf.addr)
                {
                    printf("atiny_malloc failed!");
                    p1 = p2 + data_len;
                    data_len = 0;
                    continue;
                }
                
                printf("get +IPD data, len = %d, store to :%p\n", data_len, buf.addr);
                memcpy(buf.addr, p2, data_len);
                if (LOS_OK != (ret = LOS_QueueWriteCopy(puwQueueID, &buf, sizeof(QUEUE_BUFF), 0)))
                {
                    printf("LOS_QueueWriteCopy  failed! ret = %x\n", ret);
                    atiny_free(buf.addr);
                }
               
                p1 = p2 + data_len;
                data_len = 0;
            }
            else 
            {
                p2 = strstr(p1, "\r\n+IPD");
                if (NULL == p2) {
                    
                    p2 = (char *)tmp + recv_len;//end
                    store_resp_buf((uint8_t*)p1 , p2 - p1);
                    LOS_SemPost(sim_resp_sem);
                    break;
                }
                store_resp_buf((uint8_t*)p1 , p2 - p1);
                LOS_SemPost(sim_resp_sem);
                p1 = p2;
            }
        
        }
#if 0
        if (strstr((const char *)tmp, "+IPD")){
            SIM_LOG("get +IPD data");
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
                for(i=0;i<get_data_len;i++)
                  printf("%x ",data_buf[i]);
                printf("\n");
                LOS_SemPost(sim_recv_sem);
             }
         }
         else {
            store_resp_buf(tmp, recv_len);
            LOS_SemPost(sim_resp_sem);
         }
#endif

    }
    mutex_init_failed:
        LOS_QueueDelete(puwQueueID);
    queue_init_failed:
        LOS_SemDelete(sim_resp_sem);
    resp_sem_init_failed:
        LOS_SemDelete(sim_usart_irq_sem);
    usart_irq_sem_init_failed:
        return ;
}

UINT32 create_usart_recv_thread()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 0;
    task_init_param.pcName = "task_usart_sim_recv";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)task_sim;
    task_init_param.uwStackSize = 0x1000;

    uwRet = LOS_TaskCreate(&g_simTskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;

}



void sim900a_ini(void)
{
	//检测模块响应是否正常
    while(0 == sim_usart_cmd((int8_t*)"AT\r",strlen("AT\r"),"OK",NULL))
    {
      printf("\r\n模块响应测试不正常！！\r\n");
      printf("\r\n若模块响应测试一直不正常，请检查模块的连接或是否已开启电源开关\r\n");
      SIM900A_DELAY(500);
    }
    if(sim_usart_cmd((int8_t*)"AT+CPIN?\r",strlen("AT+CPIN?\r"),"OK",NULL))
    {
      printf("检测到SIM卡\n");
    }
    if(sim_usart_cmd((int8_t*)"AT+COPS?\r",strlen("AT+COPS?\r"),"CHINA MOBILE",NULL))
    {
      printf("已注册到网络\n");
    }
		//发送短信测试
		//sim900a_sms_test();
}

void sim_usart_init()
{
    SIM_USART_Config();  
    create_usart_recv_thread();
    sim900a_ini();
}

void wireless_modu_register()
{
    hal_wireless_modu_register(&sim_modu);
    printf("register wireless module(%s) complete.\n", sim_modu.name);
}

#endif //#if NETWORK_TYPE == SIM_900A
