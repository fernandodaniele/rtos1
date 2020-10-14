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

#define LED_RATE pdMS_TO_TICKS(500)
#define LOADING_RATE pdMS_TO_TICKS(250)

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

DEBUG_PRINT_ENABLE;

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

// Prototipo de funcion de la tarea
void heart_beat( void* taskParmPtr ); // Prendo A - Apago A
void loading_1( void* taskParmPtr );  // Prendo A - Apago A - Prendo B - Apago B .... hasta D
void loading_2( void* taskParmPtr ); // Prendo A - Prendo B - ... - Apago A - Apago B - ...

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )
{
    // ---------- CONFIGURACIONES ------------------------------

    boardConfig();									// Inicializar y configurar la plataforma

    debugPrintConfigUart( UART_USB, 115200 );		// UART for debug messages
    printf( "Ejercicio B_6.\r\n" );

    //gpioWrite( LED3, ON );							// Led para dar señal de vida

    // Crear tarea en freeRTOS
    BaseType_t res =
    xTaskCreate(
    	loading_1,                     	// Funcion de la tarea a ejecutar
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
    vTaskDelay( 1000 / portTICK_RATE_MS );	 					// Envia la tarea al estado bloqueado durante 1 s (delay)
    //vTaskDelay ( 2*LED_RATE );								// Idem anterior pero mas elegante
    gpioWrite( LED1, OFF );


    TickType_t xPeriodicity =  1000 / portTICK_RATE_MS;			// Tarea periodica cada 1000 ms
    //TickType_t xPeriodicity =  2*LED_RATE;					// Idem anterior pero mas elegante

    TickType_t xLastWakeTime = xTaskGetTickCount();

    // ---------- REPETIR POR SIEMPRE --------------------------
    while( TRUE )
    {
        gpioWrite( LEDB , ON );
        vTaskDelay( 500 / portTICK_RATE_MS );
        //vTaskDelay( LED_RATE);							// Idem anterior pero mas elegante
        gpioWrite( LEDB ,OFF );

        //vTaskDelay( 500 / portTICK_RATE_MS ); //NO USAR!!

        // Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
        vTaskDelayUntil( &xLastWakeTime , xPeriodicity );
    }
}

// Implementacion de funcion de la tarea
void loading_1( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------


    TickType_t xPeriodicity =  2*LOADING_RATE;					// Idem anterior pero mas elegante

    TickType_t xLastWakeTime = xTaskGetTickCount();

    gpioMap_t led = LEDB;

    // ---------- REPETIR POR SIEMPRE --------------------------
    while( TRUE )
    {
        gpioWrite( led , ON );
        vTaskDelay( LOADING_RATE );
        gpioWrite( led ,OFF );

        //vTaskDelay( 500 / portTICK_RATE_MS ); //NO USAR!!

        if (led == LED3)
        	led = LEDB;
        else
        	led = led + 1;

        // Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
        vTaskDelayUntil( &xLastWakeTime , xPeriodicity );
    }
}

// Implementacion de funcion de la tarea
void loading_2( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------


    TickType_t xPeriodicity =  2*LOADING_RATE;					// Idem anterior pero mas elegante

    TickType_t xLastWakeTime = xTaskGetTickCount();

    gpioMap_t led = LEDB;

    // ---------- REPETIR POR SIEMPRE --------------------------
    while( TRUE )
    {
    	gpioToggle(led);

        //vTaskDelay( 500 / portTICK_RATE_MS ); //NO USAR!!

        if (led == LED3)
        	led = LEDB;
        else
        	led = led + 1;

        // Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
        vTaskDelayUntil( &xLastWakeTime , xPeriodicity/2 );
    }
}



/*==================[fin del archivo]========================================*/
