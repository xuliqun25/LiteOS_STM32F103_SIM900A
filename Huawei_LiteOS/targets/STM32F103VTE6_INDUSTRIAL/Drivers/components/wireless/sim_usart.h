#ifndef __USART2_H
#define	__USART2_H

#include "stm32f1xx.h"
#include <stdio.h>
#include "cmsis_os.h"

#define SIM_USART                  USART2

#define SIM_USART_DEBUG
#ifdef SIM_USART_DEBUG
#define SIM_LOG(fmt, arg...) do {printf("[%s:%d]"fmt"\n", __func__, __LINE__, ##arg);}while(0)
#else
#define SIM_LOG(fmt, arg...)
#endif

#define     SIM900A_DELAY(time)             osDelay(time)                 //—” ±


#endif /* __USART2_H */

