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

#define TEC_FALL        0
#define TEC_RISE        1

/*=====[Prototypes (declarations) of private functions]======================*/
static void keys_isr_config( void );

static void buttonPressed( t_key_isr_signal* event_data );
static void buttonReleased( t_key_isr_signal* event_data );
void user_buttonPressed( t_key_isr_signal* event_data );
void user_buttonReleased( t_key_isr_signal* event_data );

/*=====[Definitions of private global variables]=============================*/

/*=====[Definitions of public global variables]==============================*/

const t_key_config  keys_config[] = { [TEC1_INDEX]= {TEC1}, [TEC2_INDEX]= {TEC2}, [TEC3_INDEX]= {TEC3}, [TEC4_INDEX]= {TEC4} } ;

#define key_count   sizeof(keys_config)/sizeof(keys_config[TEC1_INDEX])

t_key_data keys_data[key_count];



xQueueHandle isr_queue; //almacenara el evento en una cola

/*=====[prototype of private functions]=================================*/
void task_tecla( void* taskParmPtr );

/*=====[Implementations of public functions]=================================*/
TickType_t get_diff( uint32_t index )
{
    TickType_t tiempo;

    taskENTER_CRITICAL();
    tiempo = keys_data[index].time_diff;
    taskEXIT_CRITICAL();

    return tiempo;
}

void clear_diff()
{
    taskENTER_CRITICAL();
    keys_data[TEC1_INDEX].time_diff = 0;
    taskEXIT_CRITICAL();
}



void keys_Init( void )
{
    BaseType_t res;

    isr_queue      = xQueueCreate( 10,sizeof(   t_key_isr_signal ) ) ;

    configASSERT( isr_queue != NULL );

    for( int i; i < key_count ; i++ )
    {
        keys_data[i].state         = BUTTON_UP;  // Set initial state
        keys_data[i].time_down      = KEYS_INVALID_TIME;
        keys_data[i].time_up        = KEYS_INVALID_TIME;
        keys_data[i].time_diff      = KEYS_INVALID_TIME;

        //keys_data[i].pressed_signal = xSemaphoreCreateBinary();


        configASSERT( keys_data[i].pressed_signal != NULL );
    }

    // Crear tareas en freeRTOS
    res = xTaskCreate (
              task_tecla,					// Funcion de la tarea a ejecutar
              ( const char * )"task_tecla",	// Nombre de la tarea como String amigable para el usuario
              configMINIMAL_STACK_SIZE*2,	// Cantidad de stack de la tarea
              0,							// Parametros de tarea
              tskIDLE_PRIORITY+1,			// Prioridad de la tarea
              0							// Puntero a la tarea creada en el sistema
          );


    keys_isr_config();


    // Gestión de errores
    configASSERT( res == pdPASS );
}

void keys_Update_Isr( t_key_isr_signal* event_data )
{
    uint32_t index = event_data->tecla;

    switch( keys_data[index].state )
    {
        case STATE_BUTTON_UP:

            if( event_data->event_type == TEC_FALL )
            {
                vTaskDelay( DEBOUNCE_TIME / portTICK_RATE_MS );

                if( !gpioRead( keys_config[index].tecla ) )
                {
                    keys_data[index].state = STATE_BUTTON_DOWN;

                    /* ACCION DEL EVENTO !*/
                    buttonPressed( event_data );
                }
            }
            break;


        case STATE_BUTTON_DOWN:

            if( event_data->event_type == TEC_RISE )
            {
                vTaskDelay( DEBOUNCE_TIME / portTICK_RATE_MS );

                if(  gpioRead( keys_config[index].tecla ) )
                {
                    keys_data[index].state = STATE_BUTTON_UP;

                    /* ACCION DEL EVENTO !*/
                    buttonReleased( event_data );
                }
            }

            break;
    }
}



/*=====[Implementations of private functions]================================*/

/* accion de el evento de tecla pulsada */
static void buttonPressed( t_key_isr_signal* event_data )
{
    uint32_t index = event_data->tecla;

    //internal event
    taskENTER_CRITICAL();
    keys_data[index].time_down = event_data->event_time;
    taskEXIT_CRITICAL();

    //user event
    user_buttonPressed( event_data );
}

/* accion de el evento de tecla liberada */
static void buttonReleased( t_key_isr_signal* event_data )
{
    uint32_t index = event_data->tecla;
    //internal event
    taskENTER_CRITICAL();
    keys_data[index].time_up    = event_data->event_time;
    keys_data[index].time_diff  = keys_data[index].time_up - keys_data[index].time_down;
    taskEXIT_CRITICAL();

    //user event
    user_buttonReleased( event_data );
}

/*=====[Implementations of private functions]=================================*/
void task_tecla( void* taskParmPtr )
{
    t_key_isr_signal event_data;

    while( 1 )
    {
        xQueueReceive( isr_queue, &event_data, portMAX_DELAY );
        keys_Update_Isr( &event_data );
    }
}


/**
   @brief   Inicializa las interrupciones asociadas al driver keys.c
			Se realiza sobre las cuatro teclas de la EDUCIAA
 */
void keys_isr_config( void )
{
    //Inicializamos las interrupciones (LPCopen)
    Chip_PININT_Init( LPC_GPIO_PIN_INT );

    //Inicializamos de cada evento de interrupcion (LPCopen)

    /* Machete:
    GLOBAL! extern pinInitGpioLpc4337_t gpioPinsInit[];
    Chip_SCU_GPIOIntPinSel( j,  gpioPinsInit[i].gpio.port, gpioPinsInit[i].gpio.pin );   // TECi
    Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( j ) );                      // INTj (canal j -> hanlder GPIOj)       //Borra el pending de la IRQ
    Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( j ) );                      // INTj //Selecciona activo por flanco
    Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH( j ) );                        // INTj //Selecciona activo por flanco descendente
    Chip_PININT_EnableIntHigh( LPC_GPIO_PIN_INT, PININTCH( j ) );                       // INTj //Selecciona activo por flanco ascendente
    */


    // TEC1 FALL +  TEC1 RISE
    Chip_SCU_GPIOIntPinSel( 0, 0, 4 ); 	//(Canal 0 a 7, Puerto GPIO, Pin GPIO)
    Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH0 ); //Se configura el canal para que se active por flanco
    Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH0 ); //Se configura para que el flanco sea el de bajada
    Chip_PININT_EnableIntHigh( LPC_GPIO_PIN_INT, PININTCH0 ); //En este caso el flanco es de subida

    // TEC2 FALL + TEC2 RISE
    Chip_SCU_GPIOIntPinSel( 1, 0, 8 );
    Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH1 );
    Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH1 );
    Chip_PININT_EnableIntHigh( LPC_GPIO_PIN_INT, PININTCH1 );


    // TEC3 FALL + TEC3 RISE
    Chip_SCU_GPIOIntPinSel( 2, 0, 9 );
    Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH2 );
    Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH2 );
    Chip_PININT_EnableIntHigh( LPC_GPIO_PIN_INT, PININTCH2 );

    // TEC4 FALL + TEC4 RISE
    Chip_SCU_GPIOIntPinSel( 3, 1, 9 );
    Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH3 );
    Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH3 );
    Chip_PININT_EnableIntHigh( LPC_GPIO_PIN_INT, PININTCH3 );

    //Una vez que se han configurado los eventos para cada canal de interrupcion
    //Se activan las interrupciones para que comiencen a llamar al handler
    NVIC_SetPriority( PIN_INT0_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY );
    NVIC_EnableIRQ( PIN_INT0_IRQn );
    NVIC_SetPriority( PIN_INT1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY );
    NVIC_EnableIRQ( PIN_INT1_IRQn );
    NVIC_SetPriority( PIN_INT2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY );
    NVIC_EnableIRQ( PIN_INT2_IRQn );
    NVIC_SetPriority( PIN_INT3_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY );
    NVIC_EnableIRQ( PIN_INT3_IRQn );
}



/**
   @brief handler de evento de tecla pulsada

   @param index
 */
void keys_isr_fall( uint32_t index, t_key_isr_signal*evnt )
{
    evnt->tecla         = index;
    evnt->event_time    = xTaskGetTickCountFromISR();
    evnt->event_type    = TEC_FALL;
}

/**
   @brief handler de evento de tecla liberada

   @param index
 */
void keys_isr_rise( uint32_t index, t_key_isr_signal*evnt )
{
    evnt->tecla         = index;
    evnt->event_time    = xTaskGetTickCountFromISR();
    evnt->event_type    = TEC_RISE;
}

void GPIO0_IRQHandler( void )   //asociado a tec1
{
    t_key_isr_signal event_data;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; //Comenzamos definiendo la variable

    if ( Chip_PININT_GetFallStates( LPC_GPIO_PIN_INT ) & PININTCH0 )
    {
        //Verificamos que la interrupción es la esperada
        Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH0 ); //Borramos el flag de interrupción

        keys_isr_fall( TEC1_INDEX, &event_data );
        xQueueSendFromISR(  isr_queue, &event_data, &xHigherPriorityTaskWoken );
    }

    if ( Chip_PININT_GetRiseStates( LPC_GPIO_PIN_INT ) & PININTCH0 )
    {
        Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH0 );
        keys_isr_rise( TEC1_INDEX, &event_data );
        xQueueSendFromISR( isr_queue, &event_data, &xHigherPriorityTaskWoken );
    }

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void GPIO1_IRQHandler( void )  //asociado a tec1
{
    t_key_isr_signal event_data;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; //Comenzamos definiendo la variable

    if ( Chip_PININT_GetFallStates( LPC_GPIO_PIN_INT ) & PININTCH1 )
    {
        //Verificamos que la interrupción es la esperada
        Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH1 ); //Borramos el flag de interrupción

        keys_isr_fall( TEC2_INDEX, &event_data );
        xQueueSendFromISR( isr_queue, &event_data, &xHigherPriorityTaskWoken );
    }

    if ( Chip_PININT_GetRiseStates( LPC_GPIO_PIN_INT ) & PININTCH1 )
    {
        Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH1 );
        keys_isr_rise( TEC2_INDEX, &event_data );
        xQueueSendFromISR( isr_queue, &event_data, &xHigherPriorityTaskWoken );
    }

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


void GPIO2_IRQHandler( void )
{
    t_key_isr_signal event_data;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; //Comenzamos definiendo la variable

    if ( Chip_PININT_GetFallStates( LPC_GPIO_PIN_INT ) & PININTCH2 )
    {
        //Verificamos que la interrupción es la esperada
        Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH2 ); //Borramos el flag de interrupción

        keys_isr_fall( TEC3_INDEX, &event_data );
        xQueueSendFromISR( isr_queue, &event_data, &xHigherPriorityTaskWoken );
    }

    if ( Chip_PININT_GetRiseStates( LPC_GPIO_PIN_INT ) & PININTCH2 )
    {
        Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH2 );
        keys_isr_rise( TEC3_INDEX, &event_data );
        xQueueSendFromISR( isr_queue, &event_data, &xHigherPriorityTaskWoken );
    }

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void GPIO3_IRQHandler( void )
{
    t_key_isr_signal event_data;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; //Comenzamos definiendo la variable

    if ( Chip_PININT_GetFallStates( LPC_GPIO_PIN_INT ) & PININTCH3 )
    {
        //Verificamos que la interrupción es la esperada
        Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH3 ); //Borramos el flag de interrupción

        keys_isr_fall( TEC4_INDEX, &event_data );
        xQueueSendFromISR( isr_queue, &event_data, &xHigherPriorityTaskWoken );
    }

    if ( Chip_PININT_GetRiseStates( LPC_GPIO_PIN_INT ) & PININTCH3 )
    {
        Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH3 );
        keys_isr_rise( TEC4_INDEX, &event_data );
        xQueueSendFromISR( isr_queue, &event_data, &xHigherPriorityTaskWoken );
    }

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}



