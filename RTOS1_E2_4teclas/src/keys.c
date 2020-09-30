/* Copyright 2020, Franco Bucafusco
 * All rights reserved.
 *
 * This file is part of sAPI Library.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*==================[ Inclusions ]============================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "keys.h"

/*=====[ Definitions of private data types ]===================================*/


/*=====[Definition macros of private constants]==============================*/
//#define BUTTON_RATE     1
#define DEBOUNCE_TIME   40

/*=====[Prototypes (declarations) of private functions]======================*/

static void keys_ButtonError( uint32_t index );
static void buttonPressed( uint32_t index );
static void buttonReleased( uint32_t index );

/*=====[Definitions of private global variables]=============================*/

/*=====[Definitions of public global variables]==============================*/
const t_key_config  keys_config[] = { TEC1, TEC2, TEC3, TEC4 } ;
gpioMap_t leds[]   = {LEDB,LED1,LED2,LED3};

TickType_t c1 = 500;

#define key_count   sizeof(keys_config)/sizeof(keys_config[0])

t_key_data keys_data[key_count];

tLedTecla tecla_led_config[key_count];

QueueHandle_t queue_tec_pulsada[key_count];

/*=====[prototype of private functions]=================================*/
void task_tecla( void* taskParmPtr );
void task_led( void* taskParmPtr );

/*=====[Implementations of public functions]=================================*/
TickType_t get_diff()
{
	TickType_t tiempo;

	taskENTER_CRITICAL();
	tiempo = keys_data[0].time_diff;
	taskEXIT_CRITICAL();

	return tiempo;
}

TickType_t get_c1()
{
	return c1;
}

void inc_c1()
{
	taskENTER_CRITICAL();
	c1 = c1 + 100;
	if (c1 > 1900)
	{
		c1 = 1900;
	}
	taskEXIT_CRITICAL();
}

void clear_diff()
{
	taskENTER_CRITICAL();
	keys_data[0].time_diff = 0;
	taskEXIT_CRITICAL();
}

void keys_Init( void )
{
	BaseType_t res;

	for(int i=0; i <sizeof(keys_data);i++)
	{
		keys_data[i].state          = BUTTON_UP;  // Set initial state
		keys_data[i].time_down      = KEYS_INVALID_TIME;
		keys_data[i].time_up        = KEYS_INVALID_TIME;
		keys_data[i].time_diff      = KEYS_INVALID_TIME;
	}
	// Crear tareas en freeRTOS
	res = xTaskCreate (
			  task_tecla,					// Funcion de la tarea a ejecutar
			  ( const char * )"task_tecla",	// Nombre de la tarea como String amigable para el usuario
			  configMINIMAL_STACK_SIZE*2,	// Cantidad de stack de la tarea
			  tecla_led_config,							// Parametros de tarea
			  tskIDLE_PRIORITY+1,			// Prioridad de la tarea
			  0							// Puntero a la tarea creada en el sistema
		  );

	// Gestión de errores
	configASSERT( (res) == pdPASS );
}

void leds_Init( void )
{
	BaseType_t res;

	// Crear tareas en freeRTOS
	res = xTaskCreate (
			  task_led,					// Funcion de la tarea a ejecutar
			  ( const char * )"task_led",	// Nombre de la tarea como String amigable para el usuario
			  configMINIMAL_STACK_SIZE*2,	// Cantidad de stack de la tarea
			  tecla_led_config,							// Parametros de tarea
			  tskIDLE_PRIORITY+1,			// Prioridad de la tarea
			  0							// Puntero a la tarea creada en el sistema
		  );

	// Gestión de errores
	configASSERT( (res) == pdPASS );
}

void tecla_led_init(void)
{
	uint16_t i;

	for(i = 0 ; i < key_count ; i++)
	{
		tecla_led_config[i].led 	= leds[i];
		tecla_led_config[i].tecla 	= keys_config[i];
		queue_tec_pulsada[i] = xQueueCreate( 1 , sizeof(TickType_t) );

		// Gestion de errores de colas
		if( queue_tec_pulsada[i]== NULL)
		{
			gpioWrite( LED_ERROR , ON );
			printf( MSG_ERROR_QUEUE );
			while(TRUE);						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
		}
	}
}

// keys_ Update State Function
void keys_Update( uint32_t index )
{
	switch( keys_data[index].state )
	{
		case STATE_BUTTON_UP:
			/* CHECK TRANSITION CONDITIONS */
			if( !gpioRead( keys_config[index].tecla ) )
			{
				keys_data[index].state = STATE_BUTTON_FALLING;
			}
			break;

		case STATE_BUTTON_FALLING:
			/* ENTRY */

			/* CHECK TRANSITION CONDITIONS */
			if( !gpioRead( keys_config[index].tecla ) )
			{
				keys_data[index].state = STATE_BUTTON_DOWN;

				/* ACCION DEL EVENTO !*/
				buttonPressed( index );
			}
			else
			{
				keys_data[index].state = STATE_BUTTON_UP;
			}

			/* LEAVE */
			break;

		case STATE_BUTTON_DOWN:
			/* CHECK TRANSITION CONDITIONS */
			if( gpioRead( keys_config[index].tecla ) )
			{
				keys_data[index].state = STATE_BUTTON_RISING;
			}
			break;

		case STATE_BUTTON_RISING:
			/* ENTRY */

			/* CHECK TRANSITION CONDITIONS */

			if( gpioRead( keys_config[index].tecla ) )
			{
				keys_data[index].state = STATE_BUTTON_UP;

				/* ACCION DEL EVENTO ! */
				buttonReleased( index );
			}
			else
			{
				keys_data[index].state = STATE_BUTTON_DOWN;
			}

			/* LEAVE */
			break;

		default:
			keys_ButtonError( index );
			break;
	}
}

/*=====[Implementations of private functions]================================*/

/* accion de el evento de tecla pulsada */
static void buttonPressed( uint32_t index )
{
	TickType_t current_tick_count = xTaskGetTickCount();

	taskENTER_CRITICAL();
	keys_data[index].time_down = current_tick_count;
	taskEXIT_CRITICAL();
}

/* accion de el evento de tecla liberada */
static void buttonReleased( uint32_t index )
{
	TickType_t current_tick_count = xTaskGetTickCount();

	taskENTER_CRITICAL();
	keys_data[index].time_up    = current_tick_count;
	keys_data[index].time_diff  = keys_data[index].time_up - keys_data[index].time_down;
	taskEXIT_CRITICAL();
	if (keys_data[index].time_diff > 0)
	{
		xQueueSend( &queue_tec_pulsada[index] , &(keys_data[index].time_diff),  portMAX_DELAY  );
	}
}

static void keys_ButtonError( uint32_t index )
{
	taskENTER_CRITICAL();
	keys_data[index].state = BUTTON_UP;
	taskEXIT_CRITICAL();
}

/*=====[Implementations of private functions]=================================*/
void task_tecla( void* taskParmPtr )
{
	while( 1 )
	{
		for(int i=0; i <sizeof(keys_data);i++)
		{
			keys_Update( i );
		}
		vTaskDelay( DEBOUNCE_TIME / portTICK_RATE_MS );
	}
}

void task_led( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------
	tLedTecla* config;// = (tLedTecla*) taskParmPtr;

	TickType_t xPeriodicity =  MAX_RATE;
	TickType_t xLastWakeTime = xTaskGetTickCount();
    // ---------- REPETIR POR SIEMPRE --------------------------

	TickType_t dif = 0;
	while( TRUE )
	{
		xQueueReceive( queue_tec_pulsada[0] , &dif, 0);
		if (dif > xPeriodicity)
			dif = xPeriodicity;
		gpioWrite( LEDB  , ON );
		vTaskDelay( dif );
		gpioWrite( LEDB  , OFF );

		vTaskDelayUntil( &xLastWakeTime , xPeriodicity );
	}
}
