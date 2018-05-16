/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/
#include "stdlib.h"
#include "string.h"
#include "los_base.h"
#include "los_config.h"
#include "los_typedef.h"
#include "los_hwi.h"
#include "los_task.ph"
#include "los_sem.h"
#include "los_event.h"
#include "los_memory.h"
#include "los_queue.ph"
#include "cmsis_os.h"
#include <stdio.h>


#include "sys_init.h"
#include "los_inspect_entry.h"
#include "los_demo_debug.h"

#include "netconf.h"

#if NETWORK_TYPE != COMMON_ETHER
	#include "wireless.h"
#endif

//variables
UINT32 global_TaskHandle = 0;
extern UART_HandleTypeDef SIM_UsartHandle;
//function prototype
UINT32 creat_task_test();
VOID HardWare_Init(VOID);
UINT32 creat_task_test();
VOID task_test();
extern void SIM_USART_Config();
extern void sim_usart_init();

/**
	*@breif hardware configuration
	*@params none
	*@ret none
*/
VOID HardWare_Init(VOID)
{
    SystemClock_Config();
    Debug_UART_Init();
	  SIM_USART_Config();
}



/**
  *@brief  main body of task_test
  *@params none
*/
VOID task_test()
{		
//	UINT32 uwcyclePerTick;
//	UINT64 uwTickCount1,uwTickCount2;

//	for(;;)
//	{
//		uwcyclePerTick  = LOS_CyclePerTickGet();
//		if(0 != uwcyclePerTick)
//		{
//			dprintf("LOS_CyclePerTickGet = %d \n", uwcyclePerTick);
//		}

//		uwTickCount1 = LOS_TickCountGet();
//		if(0 != uwTickCount1)
//		{
//			printf("LOS_TickCountGet = %d \n", (UINT32)uwTickCount1);
//		}
//		
//		LOS_TaskDelay(200);
//		uwTickCount2 = LOS_TickCountGet();
//		if(0 != uwTickCount2)
//		{
//			printf("LOS_TickCountGet after delay = %d \n", (UINT32)uwTickCount2);
//		}
//	}
	#if 1
	//printf("hello_world\n");
	wireless_modu_register();
	hal_wireless_init();
	extern void agent_tiny_entry(void);
	agent_tiny_entry();
	#endif
}


/**
  * @brief create a task to test kernel
  * @param none
*/
UINT32 creat_task_test()
{
	UINT32 uwRet = LOS_OK;
  TSK_INIT_PARAM_S task_init_param;

  task_init_param.usTaskPrio = 1;
  task_init_param.pcName = "task_test";
  task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)task_test;
  task_init_param.uwStackSize = 4096;   //minumun stack size is 304

  uwRet = LOS_TaskCreate(&global_TaskHandle, &task_init_param);
  if(LOS_OK != uwRet)
  {
		return uwRet;
  }
	
  return uwRet;
}

/**
	*@breif main function entry
	*@params none
  *@ret int value
*/
int main(void)
{
	UINT32 uwRet = LOS_OK;

	//HardWare_Init();
	
	printf("++++++++++++test++++++++++++\n");

	uwRet = LOS_KernelInit();
	if (uwRet != LOS_OK)
	{
			return LOS_NOK;
	}
	HardWare_Init();

	//LOS_Inspect_Entry();
	
	uwRet = creat_task_test();

	if(uwRet != LOS_OK)
	{
		printf("create test task failed\n\r");
		return uwRet;
	}

	LOS_Start();
}




