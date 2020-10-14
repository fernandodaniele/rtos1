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

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[tareas]====================*/

void tarea_tecla( void* taskParmPtr )
{
	tLedTecla* config = (tLedTecla*) taskParmPtr;

	fsmButtonInit( config );

	while( TRUE )
	{
		fsmButtonUpdate( config );
		vTaskDelay( BUTTON_RATE );
	}
}

void tarea_led_a( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------
	tLedTecla* config = (tLedTecla*) taskParmPtr;

	TickType_t xPeriodicity =  MAX_RATE;
	TickType_t xLastWakeTime = xTaskGetTickCount();
    // ---------- REPETIR POR SIEMPRE --------------------------

	TickType_t dif = 0;
	while( TRUE )
	{
		xSemaphoreTake( config->mutex , portMAX_DELAY );			//abrir seccion critica
		//xLastWakeTime = xTaskGetTickCount();

		gpioWrite( config->led , ON );
		vTaskDelay( xPeriodicity / 2 );
		gpioWrite( config->led , OFF );

		xSemaphoreGive( config->mutex );							//cerrar seccion critica

		vTaskDelayUntil( &xLastWakeTime , xPeriodicity );
	}
}

void tarea_led_b( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------
	tLedTecla* config = (tLedTecla*) taskParmPtr;

	TickType_t xPeriodicity =  MAX_RATE;
	TickType_t xLastWakeTime = xTaskGetTickCount();
    // ---------- REPETIR POR SIEMPRE --------------------------

	TickType_t dif = 0;
	while( TRUE )
	{
		xQueueReceive( config->queue_tec_pulsada , &dif,  portMAX_DELAY );			// Esperamos tecla

		if (dif > xPeriodicity)
			dif = xPeriodicity;

		xSemaphoreTake( config->mutex , portMAX_DELAY );			//abrir seccion critica
		gpioWrite( config->led , ON );
		vTaskDelay( dif );
		gpioWrite( config->led , OFF );
		xSemaphoreGive( config->mutex );							//cerrar seccion critica
	}
}

