/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/09/03
 * Version: v1.1
 *===========================================================================*/
#ifndef _FSM_H_
#define _FSM_H_

/*==================[inclusiones]============================================*/
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "semphr.h"
/*==================[definiciones y macros]==================================*/
#define DEBOUNCE_TIME 40
/*==================[definiciones de datos]=========================*/
// Tipo de dato FSM
typedef enum
{
    STATE_BUTTON_UP,
    STATE_BUTTON_DOWN,
    STATE_BUTTON_FALLING,
    STATE_BUTTON_RISING
} fsmButtonState_t;

// Estructura principal
typedef struct
{
	gpioMap_t tecla;			//config
	gpioMap_t led;				//config

	TickType_t tiempo_medido;	//variables
	fsmButtonState_t fsmButtonState;//variables
	TickType_t tiempo_down;		//variables

	uint8_t contFalling  ;		//variables
	uint8_t contRising  ;		//variables

	QueueHandle_t queue_tec_pulsada;		//cola
} tLedTecla;

typedef struct
{
	gpioMap_t tecla;				//config
	TickType_t tiempo_medido;	//variables
} tPrint;


/*==================[prototipos de funciones]====================*/
TickType_t get_diff();
void clear_diff();

void fsmButtonError( tLedTecla* config );
void fsmButtonInit( tLedTecla* config );
void fsmButtonUpdate( tLedTecla* config );
void buttonPressed( tLedTecla* config );
void buttonReleased( tLedTecla* config );

#endif /* _FSM_H_ */
