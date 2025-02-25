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
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "chip.h"

#define configUSE_PREEMPTION                         1
#define configUSE_TIME_SLICING                       0 //deshabilito round robin


#define configUSE_TICKLESS_IDLE                      0
#define configCPU_CLOCK_HZ                           ( SystemCoreClock )
#define configTICK_RATE_HZ                           ( ( TickType_t ) 1000 ) // 1000 ticks per second => 1ms tick rate
#define configMAX_PRIORITIES                         ( 7 )
#define configMINIMAL_STACK_SIZE                     90
#define configTOTAL_HEAP_SIZE                        ( ( size_t ) ( 8 * 1024 ) )    /* 85 Kbytes. */
#define configMAX_TASK_NAME_LEN                      ( 16 )
#define configUSE_TRACE_FACILITY                     0
#define configUSE_16_BIT_TICKS                       0
#define configIDLE_SHOULD_YIELD                      1
#define configUSE_MUTEXES                            0
#define configQUEUE_REGISTRY_SIZE                    8

#define configUSE_RECURSIVE_MUTEXES                  0

#define configUSE_APPLICATION_TASK_TAG               0
#define configUSE_COUNTING_SEMAPHORES                0
#define configGENERATE_RUN_TIME_STATS                0
#define configSUPPORT_STATIC_ALLOCATION              0
#define configOVERRIDE_DEFAULT_TICK_CONFIGURATION    1
#define configRECORD_STACK_HIGH_ADDRESS              1

// Hooks
#define configUSE_DAEMON_TASK_STARTUP_HOOK           0
#define configUSE_MALLOC_FAILED_HOOK                 0
#define configUSE_IDLE_HOOK                          0
#define configUSE_TICK_HOOK                          0

// runtime checks
#define configCHECK_FOR_STACK_OVERFLOW               2                                        /* INCLUIRLO PARA MEDIR EL STACK  */

// Add old API compatibility
#define configENABLE_BACKWARD_COMPATIBILITY          1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                        0
#define configMAX_CO_ROUTINE_PRIORITIES              ( 2 )

/* Software timer definitions. */
#define configUSE_TIMERS                             0
#define configTIMER_TASK_PRIORITY                    ( configMAX_PRIORITIES - 3 )
#define configTIMER_QUEUE_LENGTH                     10
#define configTIMER_TASK_STACK_DEPTH                 ( configMINIMAL_STACK_SIZE * 4 )

/* Set the following definitions to 1 to include the API function, or zero
 * to exclude the API function. */
#define INCLUDE_vTaskPrioritySet                     0
#define INCLUDE_uxTaskPriorityGet                    1
#define INCLUDE_vTaskDelete                          1
#define INCLUDE_vTaskCleanUpResources                0
#define INCLUDE_vTaskSuspend                         1
#define INCLUDE_vTaskDelayUntil                      0
#define INCLUDE_vTaskDelay                           0
#define INCLUDE_xTaskGetSchedulerState               0
#define INCLUDE_xTimerPendFunctionCall               0
#define INCLUDE_xSemaphoreGetMutexHolder             0
#define INCLUDE_uxTaskGetStackHighWaterMark          1                                      /* INCLUIRLO PARA MEDIR EL STACK  */


/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
/* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
#define configPRIO_BITS    __NVIC_PRIO_BITS
#else
#define configPRIO_BITS    3                                 /* 8 priority levels. */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
 * function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         0x7

/* The highest interrupt priority that can be used by any interrupt service
 * routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
 * INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
 * PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
* to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY \
	( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )

/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
 * See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )

/* Normal assert() semantics without relying on the provision of an assert.h
 * header file. */
#define configASSERT( x )    if( ( x ) == 0 ) {  taskDISABLE_INTERRUPTS(); for( ;; ) {; }  	}

/* Map the FreeRTOS printf() to the logging task printf. */
#define configPRINTF( x )          vLoggingPrintf x

/* Map the logging task's printf to the board specific output function. */
#define configPRINT_STRING    DbgConsole_Printf

/* Sets the length of the buffers into which logging messages are written - so
 * also defines the maximum length of each log message. */
#define configLOGGING_MAX_MESSAGE_LENGTH            100

/* Set to 1 to prepend each log message with a message number, the task name,
 * and a time stamp. */
#define configLOGGING_INCLUDE_TIME_AND_TASK_NAME    1

/* Demo specific macros that allow the application writer to insert code to be
 * executed immediately before the MCU's STOP low power mode is entered and exited
 * respectively.  These macros are in addition to the standard
 * configPRE_SLEEP_PROCESSING() and configPOST_SLEEP_PROCESSING() macros, which are
 * called pre and post the low power SLEEP mode being entered and exited.  These
 * macros can be used to turn turn off and on IO, clocks, the Flash etc. to obtain
 * the lowest power possible while the tick is off. */
#if defined( __ICCARM__ ) || defined( __CC_ARM ) || defined( __GNUC__ )
void vMainPreStopProcessing( void );
void vMainPostStopProcessing( void );
#endif /* defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__) */

#define configPRE_STOP_PROCESSING     vMainPreStopProcessing
#define configPOST_STOP_PROCESSING    vMainPostStopProcessing

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
 * standard names. */
#define vPortSVCHandler               SVC_Handler
#define xPortPendSVHandler            PendSV_Handler
#define xPortSysTickHandler           SysTick_Handler
#define vHardFault_Handler            HardFault_Handler

/* IMPORTANT: This define MUST be commented when used with STM32Cube firmware,
 *            to prevent overwriting SysTick_Handler defined within STM32Cube HAL. */
/* #define xPortSysTickHandler SysTick_Handler */

/*********************************************
 * FreeRTOS specific demos
 ********************************************/

/* The address of an echo server that will be used by the two demo echo client
 * tasks.
 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCP_Echo_Clients.html
 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/UDP_Echo_Clients.html */
#define configECHO_SERVER_ADDR0       192
#define configECHO_SERVER_ADDR1       168
#define configECHO_SERVER_ADDR2       2
#define configECHO_SERVER_ADDR3       6
#define configTCP_ECHO_CLIENT_PORT    7

/* Prevent the assembler seeing code it doesn't understand. */
#ifdef __ICCARM__
/* Logging task definitions. */
extern void vMainUARTPrintString( char * pcString );
void vLoggingPrintf( const char * pcFormat,
                     ... );

extern int iMainRand32( void );

/* Pseudo random number generator, just used by demos so does not have to be
 * secure.  Do not use the standard C library rand() function as it can cause
 * unexpected behaviour, such as calls to malloc(). */
#define configRAND32()    iMainRand32()
#endif




/* Ensure stdint is only used by the compiler, and not the assembler. */
#if defined( __ICCARM__ ) || defined( __ARMCC_VERSION )
#include <stdint.h>
extern uint32_t SystemCoreClock;
extern int DbgConsole_Printf( const char *fmt_s, ... );
#endif

#endif /* FREERTOS_CONFIG_H */
