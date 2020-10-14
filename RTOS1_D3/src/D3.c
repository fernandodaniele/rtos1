/* Copyright 2020,  Martin Menendez / Franco Bucafusco
 * All rights reserved.
 *
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

/*==================[inlcusiones]============================================*/

// Includes de FreeRTOS
#include "FreeRTOS.h"
#include "task.h"


// sAPI header
#include "sapi.h"
#include "FreeRTOSConfig.h"

/*==================[definiciones y macros]==================================*/
#define EVIDENCIAR_PROBLEMA 1

#if EVIDENCIAR_PROBLEMA==1
#define CRITICAL_DECLARE
#define CRITICAL_CONFIG
#define CRITICAL_START
#define CRITICAL_END
#else
#include "semphr.h"
#define CRITICAL_DECLARE    SemaphoreHandle_t mutex
#define CRITICAL_CONFIG     mutex = xSemaphoreCreateMutex()
#define CRITICAL_START      xSemaphoreTake( mutex , portMAX_DELAY )
#define CRITICAL_END        xSemaphoreGive( mutex )
#endif

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

DEBUG_PRINT_ENABLE;

/*==================[declaraciones de funciones internas]====================*/
CRITICAL_DECLARE;
/*==================[declaraciones de funciones externas]====================*/

// Prototipo de funcion de la tarea
void sacerdote_a( void* taskParmPtr );
void sacerdote_b( void* taskParmPtr );
void sacerdote_c( void* taskParmPtr );
void sacerdote_d( void* taskParmPtr );

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )
{
    BaseType_t res;

    // ---------- CONFIGURACIONES ------------------------------
    // Inicializar y configurar la plataforma
    boardConfig();

    // UART for debug messages
    debugPrintConfigUart( UART_USB, 9600 );

    printf( "Ejercicio printf.\n" );

    // Crear tarea en freeRTOS
    res = xTaskCreate(
              sacerdote_a,                     // Funcion de la tarea a ejecutar
              ( const char * )"myTask",   	// Nombre de la tarea como String amigable para el usuario
              configMINIMAL_STACK_SIZE*2, 	// Cantidad de stack de la tarea
              0,                          	// Parametros de tarea
              tskIDLE_PRIORITY+1,         	// Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
              0                          		// Puntero a la tarea creada en el sistema
          );

    configASSERT( res == pdPASS ); // gestion de errores

    res = xTaskCreate(
              sacerdote_b,                     // Funcion de la tarea a ejecutar
              ( const char * )"myTask",   	// Nombre de la tarea como String amigable para el usuario
              configMINIMAL_STACK_SIZE*2, 	// Cantidad de stack de la tarea
              0,                          	// Parametros de tarea
              tskIDLE_PRIORITY+1,         	// Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
              0                          		// Puntero a la tarea creada en el sistema
          );

    configASSERT( res == pdPASS ); // gestion de errores


    res = xTaskCreate(
              sacerdote_c,                     // Funcion de la tarea a ejecutar
              ( const char * )"myTask",   	// Nombre de la tarea como String amigable para el usuario
              configMINIMAL_STACK_SIZE*2, 	// Cantidad de stack de la tarea
              0,                          	// Parametros de tarea
              tskIDLE_PRIORITY+1,         	// Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
              0                          		// Puntero a la tarea creada en el sistema
          );

    configASSERT( res == pdPASS ); // gestion de errores

    res = xTaskCreate(
              sacerdote_d,                     // Funcion de la tarea a ejecutar
              ( const char * )"myTask",   	// Nombre de la tarea como String amigable para el usuario
              configMINIMAL_STACK_SIZE*2, 	// Cantidad de stack de la tarea
              0,                          	// Parametros de tarea
              tskIDLE_PRIORITY+1,         	// Prioridad de la tarea -> Queremos que este un nivel encima de IDLE
              0                          		// Puntero a la tarea creada en el sistema
          );

    configASSERT( res == pdPASS ); // gestion de errores

    CRITICAL_CONFIG;

    // Iniciar scheduler
    vTaskStartScheduler(); // Enciende tick | Crea idle y pone en ready | Evalua las tareas creadas | Prioridad mas alta pasa a running

    // ---------- REPETIR POR SIEMPRE --------------------------
    configASSERT( 0 );

    return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

// Implementacion de funcion de la tarea
void sacerdote_a( void* taskParmPtr )
{
    // ---------- REPETIR POR SIEMPRE --------------------------
    while( TRUE )
    {
        CRITICAL_START; //abrir seccion critica
        printf( "El aldeano es ROJO \n" );
        CRITICAL_END;	//cerrar seccion critica

        vTaskDelay( 99 / portTICK_RATE_MS );
    }
}

void sacerdote_b( void* taskParmPtr )
{
    while( TRUE )
    {
        CRITICAL_START; //abrir seccion critica
        printf( "El aldeano es AZUL \n" );
        CRITICAL_END;	//cerrar seccion critica

        vTaskDelay( 77 / portTICK_RATE_MS );
    }
}

void sacerdote_c( void* taskParmPtr )
{
    while( TRUE )
    {
        CRITICAL_START; //abrir seccion critica
        printf( "El aldeano es GRIS \n" );
        CRITICAL_END;	//cerrar seccion critica

        vTaskDelay( 55 / portTICK_RATE_MS );
    }
}

void sacerdote_d( void* taskParmPtr )
{
    while( TRUE )
    {
        CRITICAL_START; //abrir seccion critica
        printf( "El aldeano es CIAN \n" );
        CRITICAL_END;	//cerrar seccion critica

        vTaskDelay( 33 / portTICK_RATE_MS );
    }
}

/*==================[fin del archivo]========================================*/
