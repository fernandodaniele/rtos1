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

/*==================[definiciones y macros]==================================*/
#define LED_RATE_MS 40
#define MAX_RATE_MS 5000
#define SEPARACION_MS 200
#define BUTTON_RATE_MS 1

#define LED_RATE pdMS_TO_TICKS(LED_RATE_MS)
#define MAX_RATE pdMS_TO_TICKS(MAX_RATE_MS)
#define T_SEPARACION pdMS_TO_TICKS(SEPARACION_MS)

#define MSG_LENGTH 20

#define BUTTON_RATE pdMS_TO_TICKS(BUTTON_RATE_MS)
/*==================[definiciones de datos]=========================*/

/*==================[prototipos de tareas]====================*/
void tarea_A( void* taskParmPtr );
void tarea_B( void* taskParmPtr );

#endif /* _Tasks_H_ */
