/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/09/03
 * Version: v1.1
 *===========================================================================*/
#ifndef _MAIN_H_
#define _MAIN_H_

/*==================[inclusiones]============================================*/
#include "FreeRTOSConfig.h"
#include "FSM.h"
#include "tasks.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"

/*==================[definiciones y macros]==================================*/
#define MSG_WELCOME "Ejercicio E_4.\r\n"
#define BAUD_RATE 115200
#define UART UART_USB

#define MSG_TECLA	"tarea_tecla_"
#define MSG_LED     "tarea_led_"

#endif /* _MAIN_H_ */
