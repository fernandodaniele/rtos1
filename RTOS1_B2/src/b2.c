/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/09/03
 * Version: v1.1
 *===========================================================================*/

/*==================[inclusiones]============================================*/

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"

#include "sapi.h"

/*==================[definiciones y macros]==================================*/

#define RATE 1000
#define LED_RATE pdMS_TO_TICKS(RATE)
#define STEP 100
#define MAX  (RATE - STEP)

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

DEBUG_PRINT_ENABLE;

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

// Prototipo de funcion de la tarea
void heart_beat( void* taskParmPtr );

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )
{
	// ---------- CONFIGURACIONES ------------------------------

	boardConfig();									// Inicializar y configurar la plataforma

	debugPrintConfigUart( UART_USB, 115200 );		// UART for debug messages
	printf( "Ejercicio B_2.\r\n" );

	//gpioWrite( LED3, ON );							// Led para dar señal de vida

	// Crear tarea en freeRTOS
	BaseType_t res =
	xTaskCreate(
		heart_beat,                     	// Funcion de la tarea a ejecutar
		( const char * )"heart_beat",   	// Nombre de la tarea como String amigable para el usuario
		configMINIMAL_STACK_SIZE*2, 		// Cantidad de stack de la tarea
		0,                          		// Parametros de tarea
		tskIDLE_PRIORITY+1,         		// Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
		0                          			// Puntero a la tarea creada en el sistema
	);

	// Gestion de errores
	if(res == pdFAIL)
	{
		gpioWrite( LEDR, ON );
		printf( "Error al crear las tareas.\r\n" );
		while(TRUE);						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
	}

	// Iniciar scheduler
	vTaskStartScheduler(); // Enciende tick | Crea idle y pone en ready | Evalua las tareas creadas | Prioridad mas alta pasa a running

	// ---------- REPETIR POR SIEMPRE --------------------------
	while( TRUE )
	{
		// Si cae en este while 1 significa que no pudo iniciar el scheduler
	}

	// NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
	// directamenteno sobre un microcontroladore y no es llamado por ningun
	// Sistema Operativo, como en el caso de un programa para PC.
	return TRUE;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

// Implementacion de funcion de la tarea
void heart_beat( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------

    gpioWrite( LED1, ON );
    vTaskDelay( LED_RATE );	 					// Envia la tarea al estado bloqueado durante 1 s (delay)
    gpioWrite( LED1, OFF );


    TickType_t xPeriodicity =  LED_RATE;		// Tarea periodica cada 1000 ms

    TickType_t xLastWakeTime = xTaskGetTickCount();

    uint16_t duty = 0;



    // ---------- REPETIR POR SIEMPRE --------------------------
    while( TRUE )
    {
    	duty += STEP;

        gpioWrite( LEDB , ON );
        vTaskDelay( duty / portTICK_RATE_MS );
        //vTaskDelay ( pdMS_TO_TICKS(duty) );
        gpioWrite( LEDB , OFF );

        if (duty == MAX)
        	duty = 0;

        // Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
        vTaskDelayUntil( &xLastWakeTime , xPeriodicity );
    }
}

/*==================[fin del archivo]========================================*/
