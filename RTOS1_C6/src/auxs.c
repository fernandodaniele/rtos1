/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/09/03
 * Version: v1.1
 *===========================================================================*/

/*==================[inclusiones]============================================*/
#include "auxs.h"
/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/
gpioMap_t teclas[] = {TEC1,TEC2,TEC3,TEC4};
gpioMap_t leds[]   = {LEDB,LED1,LED2,LED3};

#define N_TECLAS  sizeof(teclas)/sizeof(gpioMap_t)		// 4 * (gpioMap_t / gpioMap_t) = 4

tLedTecla tecla_led_config[N_TECLAS];

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[tareas]====================*/

// Funcion que crea la cantidad de tareas definida en N_TECLAS
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

// Funcion que inicializa la estructura principal
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
			gpioWrite( LED_ERROR , ON );
			printf( MSG_ERROR_SEM );
			while(TRUE);						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
		}
	}
}

// Funcion que crea y valida las tareas de FreeRTOS
void tarea_crear(TaskFunction_t tarea,const char * const nombre,uint8_t stack,void * const parametros,uint8_t prioridad,TaskHandle_t * const puntero)
{
	// Crear tarea en freeRTOS
	BaseType_t res = xTaskCreate(tarea,nombre,configMINIMAL_STACK_SIZE*stack,parametros,tskIDLE_PRIORITY+prioridad,puntero);                         		// Puntero a la tarea creada en el sistema

	// Gestion de errores
	if(res == pdFAIL)
	{
		gpioWrite( LED_ERROR , ON );
		printf( MSG_ERROR_TASK );
		while(TRUE);						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
	}
}
