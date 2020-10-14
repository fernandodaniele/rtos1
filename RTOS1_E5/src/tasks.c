/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/09/03
 * Version: v1.1
 *===========================================================================*/

/*==================[inclusiones]============================================*/
#include "tasks.h"
/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/
QueueHandle_t queue_A_B;
QueueHandle_t queue_B_A;
SemaphoreHandle_t mutex;

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[tareas]====================*/

void tarea_A( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------
	TickType_t xPeriodicity =  MAX_RATE;
	TickType_t xLastWakeTime = xTaskGetTickCount();
    // ---------- REPETIR POR SIEMPRE --------------------------

	char mensaje[MSG_LENGTH];

	xQueueSend( queue_A_B , "Comenzamos!" ,  portMAX_DELAY );

	while( TRUE )
	{
		xQueueReceive( queue_B_A , &mensaje,  portMAX_DELAY );

		xSemaphoreTake( mutex , portMAX_DELAY );			//abrir seccion critica
		printf("A] Tarea B dijo: %s\r\n",mensaje);
		xSemaphoreGive( mutex );							//cerrar seccion critica

		gpioWrite( LED2, ON );
		vTaskDelay( MAX_RATE / 2 );
		xQueueSend( queue_A_B , "Hola, soy A" ,  portMAX_DELAY );
		gpioWrite( LED2 , OFF );

		vTaskDelayUntil( &xLastWakeTime , xPeriodicity );
	}
}

void tarea_B( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------
	TickType_t xPeriodicity =  MAX_RATE;
	TickType_t xLastWakeTime = xTaskGetTickCount();
    // ---------- REPETIR POR SIEMPRE --------------------------

	char mensaje[MSG_LENGTH];

	while( TRUE )
	{
		xQueueReceive( queue_A_B , &mensaje,  portMAX_DELAY );

		xSemaphoreTake( mutex , portMAX_DELAY );			//abrir seccion critica
		printf("B] Tarea A dijo: %s\r\n",mensaje);
		xSemaphoreGive( mutex );							//cerrar seccion critica

		gpioWrite( LED3, ON );
		vTaskDelay( MAX_RATE / 2 );
		xQueueSend( queue_B_A , "Hola, soy B" ,  portMAX_DELAY );
		gpioWrite( LED3 , OFF );

		vTaskDelayUntil( &xLastWakeTime , xPeriodicity );
	}
}
