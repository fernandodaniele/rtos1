/*
 * fsm_debounce.h
 *
 *  Created on: 3 sep. 2020
 *      Author: fernando
 */
#include "sapi.h"
#ifndef MISPROGRAMAS_RTOS1_B3_INC_FSM_DEBOUNCE_H_
#define MISPROGRAMAS_RTOS1_B3_INC_FSM_DEBOUNCE_H_

void fsmButtonError( void );
void fsmButtonInit( void );
void fsmButtonUpdate( gpioMap_t tecla );
void buttonPressed( void );
void buttonReleased( void );

#endif /* MISPROGRAMAS_RTOS1_B3_INC_FSM_DEBOUNCE_H_ */
