/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/09/03
 * Version: v1.1
 *===========================================================================*/

/*==================[inclusiones]============================================*/
#include "main.h"
/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/
DEBUG_PRINT_ENABLE;

extern QueueHandle_t queue_A_B;
extern QueueHandle_t queue_B_A;
extern SemaphoreHandle_t mutex;

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )
{
    // ---------- CONFIGURACIONES ------------------------------

	boardConfig();								// Inicializar y configurar la plataforma

	debugPrintConfigUart( UART, BAUD_RATE );	// UART for debug messages
	printf( MSG_WELCOME );

	 // Crear y validar tarea en freeRTOS
	tarea_crear(tarea_A,"tarea_A",SIZE,0,PRIORITY,NULL);
	tarea_crear(tarea_B,"tarea_B",SIZE,0,PRIORITY,NULL);

	queue_A_B = xQueueCreate( N_QUEUE , sizeof(char[MSG_LENGTH]) );
	queue_B_A = xQueueCreate( N_QUEUE , sizeof(char[MSG_LENGTH]) );
	mutex 				=  xSemaphoreCreateMutex();

	// Gestion de errores de semaforos
	if( mutex == NULL)
	{
		gpioWrite( LED_ERROR , ON );
		printf( MSG_ERROR_SEM );
		while(TRUE);						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
	}

	// Gestion de errores de colas
	if( (queue_A_B || queue_B_A ) == NULL)
	{
		gpioWrite( LED_ERROR , ON );
		printf( MSG_ERROR_QUEUE );
		while(TRUE);						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
	}

    // Iniciar scheduler
    vTaskStartScheduler();				// Enciende tick | Crea idle y pone en ready | Evalua las tareas creadas | Prioridad mas alta pasa a running

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
/*==================[fin del archivo]========================================*/
