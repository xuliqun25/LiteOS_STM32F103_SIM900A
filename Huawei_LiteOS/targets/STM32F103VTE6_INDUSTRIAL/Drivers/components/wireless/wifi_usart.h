#ifndef __WIFI_USART_H__
#define __WIFI_USART_H__

#define WIFI_USART                  USART3

#define WIFI_USART_RX_GPIO_PORT     GPIOB
#define WIFI_USART_RX_GPIO_CLK      RCC_AHB1Periph_GPIOB
#define WIFI_USART_RX_GPIO_PIN      GPIO_Pin_11
#define WIFI_USART_RX_AF            GPIO_AF_USART3
#define WIFI_USART_RX_PIN_SOURCE    GPIO_PinSource11

#define WIFI_USART_TX_GPIO_PORT     GPIOB
#define WIFI_USART_TX_GPIO_CLK      RCC_AHB1Periph_GPIOB
#define WIFI_USART_TX_GPIO_PIN      GPIO_Pin_10
#define WIFI_USART_TX_AF            GPIO_AF_USART3
#define WIFI_USART_TX_PIN_SOURCE    GPIO_PinSource10

#define WIFI_USART_CLK              RCC_APB1Periph_USART3

#define WIFI_USART_BUARDRATE        115200

#define WIFI_IRQn                   USART3_IRQn

#define WIFI_USART_DEBUG
#ifdef WIFI_USART_DEBUG
#define WIFI_LOG(fmt, arg...) do {printf("[%s:%d]"fmt"\n", __func__, __LINE__, ##arg);}while(0)
#else
#define WIFI_LOG(fmt, arg...)
#endif

void wifi_modu_register(void);

typedef enum{
    STA,
    AP,
    STA_AP  
} ENUM_Net_ModeTypeDef;


#endif /* __WIFI_USART_H__ */
