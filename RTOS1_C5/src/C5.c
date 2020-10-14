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
#include "tipos.h"
#include <string.h>

/*==================[definiciones y macros]==================================*/

#define RATE 40
#define LED_RATE pdMS_TO_TICKS(RATE)
#define MAX_RATE pdMS_TO_TICKS(3000)
#define BUTTON_RATE 1
#define MAX_SEM 5

#define PRIORITY 1
#define SIZE     10
#define STR_AUX  20

/*==================[definiciones de datos internos]=========================*/

gpioMap_t teclas[] = {TEC1,TEC2,TEC3,TEC4};
gpioMap_t leds[]   = {LEDB,LED1,LED2,LED3};

#define N_TECLAS  sizeof(teclas)/sizeof(gpioMap_t)		// 4 * (gpioMap_t / gpioMap_t) = 4

tLedTecla tecla_led_config[N_TECLAS];

/*==================[definiciones de datos externos]=========================*/
DEBUG_PRINT_ENABLE;

/*==================[declaraciones de funciones internas]====================*/

void tecla_led_init(void);

void tareas_crear(TaskFunction_t tarea,const char * const nombre);

void tarea_crear(TaskFunction_t tarea,const char * const nombre,uint8_t stack,void * const parametros,uint8_t prioridad,TaskHandle_t * const puntero);


/*==================[declaraciones de funciones externas]====================*/

TickType_t get_diff();
void clear_diff();

void fsmButtonError( tLedTecla* config );
void fsmButtonInit( tLedTecla* config );
void fsmButtonUpdate( tLedTecla* config );
void buttonPressed( tLedTecla* config );
void buttonReleased( tLedTecla* config );

/*==================[prototipos de tareas]====================*/
void tarea_led( void* taskParmPtr );
void tarea_tecla( void* taskParmPtr );

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )
{
    // ---------- CONFIGURACIONES ------------------------------

	boardConfig();									// Inicializar y configurar la plataforma

	debugPrintConfigUart( UART_USB, 115200 );		// UART for debug messages
	printf( "Ejercicio C_5.\r\n" );

	//gpioWrite( LED3, ON );							// Led para dar señal de vida

	tecla_led_init();

    // Crear y validar tarea en freeRTOS
	tareas_crear(tarea_tecla,"tarea_tecla_");
	tareas_crear(tarea_led,"tarea_led_");

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

/*==================[definiciones de funciones internas]=====================*/

void tareas_crear(TaskFunction_t tarea,const char * const nombre)
{
	uint16_t i;

	char nombre_aux[STR_AUX];		// "tarea_tecla_"

	for(i = 0 ; i < N_TECLAS ; i++)
	{
		strcpy(nombre_aux, nombre);
		strcat(nombre_aux, (i+1)+'0');		// 2+'0' = '2' "tarea_tecla_2"
		//          tarea    ,nombre      ,SIZE*STACK,parametro           ,idle+1,0;
		tarea_crear(tarea,nombre_aux,SIZE,&tecla_led_config[i],PRIORITY,NULL);
	}
}


void tecla_led_init(void)
{
	uint16_t i;

	for(i = 0 ; i < N_TECLAS ; i++)
	{
		tecla_led_config[i].led 	= leds[i];
		tecla_led_config[i].tecla 	= teclas[i];
		tecla_led_config[i].sem_tec_pulsada = xSemaphoreCreateCounting( MAX_SEM , 0 );

		// Gestion de errores de semaforos
		if( tecla_led_config[i].sem_tec_pulsada == pdFALSE)
		{
			gpioWrite( LEDR, ON );
			printf( "Error al crear los semaforos.\r\n" );
			while(TRUE);						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
		}
	}
}

void tarea_crear(TaskFunction_t tarea,const char * const nombre,uint8_t stack,void * const parametros,uint8_t prioridad,TaskHandle_t * const puntero)
{
	// Crear tarea en freeRTOS
	BaseType_t res = xTaskCreate(tarea,nombre,configMINIMAL_STACK_SIZE*stack,parametros,tskIDLE_PRIORITY+prioridad,puntero);                         		// Puntero a la tarea creada en el sistema

	// Gestion de errores
	if(res == pdFAIL)
	{
		gpioWrite( LEDR, ON );
		printf( "Error al crear las tareas. \n\r" );
		while(TRUE);						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
	}
}

/*==================[definiciones de funciones externas]=====================*/

void tarea_tecla( void* taskParmPtr )
{
	tLedTecla* config = (tLedTecla*) taskParmPtr;

	fsmButtonInit( config );

	while( TRUE )
	{
		fsmButtonUpdate( config );
		vTaskDelay( BUTTON_RATE / portTICK_RATE_MS );
		//vTaskDelay ( pdMS_TO_TICKS(BUTTON_RATE) );
	}
}

// Implementacion de funcion de la tarea
void tarea_led( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------
	tLedTecla* config = (tLedTecla*) taskParmPtr;

	TickType_t xPeriodicity =  MAX_RATE;		// Tarea periodica cada 1000 ms
	TickType_t xLastWakeTime = xTaskGetTickCount();
	uint8_t contador = 0;
    // ---------- REPETIR POR SIEMPRE --------------------------

	while( TRUE )
	{
		contador = (uint8_t)uxSemaphoreGetCount(config->sem_tec_pulsada);
		printf("Quedan %d semaforos\r\n",contador);

		xSemaphoreTake( config->sem_tec_pulsada , portMAX_DELAY );			// Esperamos tecla

		xLastWakeTime = xTaskGetTickCount();

		//printf("Encendidio %d\r\n",contador);
		gpioWrite( config->led , ON );
		vTaskDelay( xPeriodicity / 2 );
		//printf("Apagado %d\r\n",contador);
		gpioWrite( config->led , OFF );
		vTaskDelayUntil( &xLastWakeTime , xPeriodicity );
	}
}

/*==================[fin del archivo]========================================*/
