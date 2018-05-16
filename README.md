# IoT2.0

1. 网络类型
    在netconf.h文件中添加定义如下:  
#define　　　　LAN8742A_ETH　　　　　　　　　0  
#define　　　　ESP8266_USART_WIFI　　　　　　1  
#define　　　　M8686_SDIO_WIFI　　　　　　　　2  
  
#define NETWORK_TYPE            ESP8265_USART_WIFI    

1. ESP8266  
    使用USART3与ESP8266模块进行连接，主要连线如下  
　　　ESP8266　　　　　　STM32F429  
　　　Vcc　　　　　　　　　3.3V  
　　　GND　　　　　　　　　GND  
　　　TXD　　　　　　　　　PB11  
　　　RXD　　　　　　　　　PB10  
