/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/09/03
 * Version: v1.1
 *===========================================================================*/
#ifndef _TASKS_H_
#define _TASKS_H_

/*==================[inclusiones]============================================*/
#include "FreeRTOS.h"
#include "sapi.h"
#include "semphr.h"
#include "auxs.h"
#include <string.h>
#include "auxs.h"
#include "FreeRTOSConfig.h"
#include "FSM.h"

/*==================[definiciones y macros]==================================*/
#define LED_RATE_MS 40
#define MAX_RATE_MS 3000
#define BUTTON_RATE_MS 1

#define LED_RATE pdMS_TO_TICKS(LED_RATE_MS)
#define MAX_RATE pdMS_TO_TICKS(MAX_RATE_MS)
#define BUTTON_RATE pdMS_TO_TICKS(BUTTON_RATE_MS)
/*==================[definiciones de datos]=========================*/
extern gpioMap_t teclas[];
extern gpioMap_t leds[];
extern tLedTecla tecla_led_config[];
/*==================[prototipos de tareas]====================*/
void tarea_led_a( void* taskParmPtr );
void tarea_led_b( void* taskParmPtr );
void tarea_tecla( void* taskParmPtr );

#endif /* _Tasks_H_ */
