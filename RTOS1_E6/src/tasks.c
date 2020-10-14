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
QueueHandle_t queue_print;
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


void tarea_led( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------
	tLedTecla* config = (tLedTecla*) taskParmPtr;

	TickType_t xPeriodicity =  MAX_RATE;
	TickType_t xLastWakeTime = xTaskGetTickCount();
    // ---------- REPETIR POR SIEMPRE --------------------------

	TickType_t dif = 0;

	tPrint datos;

	while( TRUE )
	{
		xQueueReceive( config->queue_tec_pulsada , &dif,  portMAX_DELAY );			// Esperamos tecla

		if (dif > xPeriodicity)
			dif = xPeriodicity;

		gpioWrite( config->led , ON );
		vTaskDelay( dif );

		datos.tecla = config->tecla;
		datos.tiempo_medido = dif;
		xQueueSend( queue_print , &datos ,  portMAX_DELAY );

		gpioWrite( config->led , OFF );

		vTaskDelayUntil( &xLastWakeTime , xPeriodicity );
	}
}

void tarea_print( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------
	tLedTecla* config = (tLedTecla*) taskParmPtr;

	TickType_t xPeriodicity =  PRINT_RATE;
	TickType_t xLastWakeTime = xTaskGetTickCount();
    // ---------- REPETIR POR SIEMPRE --------------------------

	tPrint datos;

	while( TRUE )
	{
		xQueueReceive( queue_print , &datos,  portMAX_DELAY );			// Esperamos tecla

		printf("Se presiono TEC%d por %d ms\r\n",datos.tecla-TEC1+1,datos.tiempo_medido*portTICK_RATE_MS);



		vTaskDelayUntil( &xLastWakeTime , xPeriodicity );
	}
}
