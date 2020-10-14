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
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "sapi.h"

#include "semphr.h"



void wait_frame( void* pvParameters )
{
    char* data;
    uint16_t size;

    uint16_t frame_counter = 0;

    while( TRUE )
    {
        protocol_wait_frame();

        protocol_get_frame_ref( &data, &size );

        /* para el printf */
        data[size] = '\0';
        printf( "%s %u\n", data, frame_counter );


        protocol_discard_frame();

        /* hago un blink para que se vea */
        gpioToggle( LEDB );
        vTaskDelay( 100/portTICK_RATE_MS );
        gpioToggle( LEDB );

        frame_counter++;
    }
}


int main( void )
{
    /* Inicializar la placa */
    boardConfig();

    procotol_x_init( UART_USB, 115200 );


    xTaskCreate(
        wait_frame,                  // Funcion de la tarea a ejecutar
        ( const char * )"wait_frame", // Nombre de la tarea como String amigable para el usuario
        configMINIMAL_STACK_SIZE*2,   // Cantidad de stack de la tarea
        0,                            // Parametros de tarea
        tskIDLE_PRIORITY+1,           // Prioridad de la tarea
        0                             // Puntero a la tarea creada en el sistema
    );

    vTaskStartScheduler();

    return 0;
}
