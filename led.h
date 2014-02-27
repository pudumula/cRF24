/*
 * Copyright (C) 2014 Prashanth.Udumula
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LED_H_
#define _LED_H_

#include<avr/io.h>

#ifdef USE_LED_TX_RX
#define LED_DDR     DDRC
#define LED_PORT    PORTC
#define LED_1       2
#define LED_ON      PORTC |= (1<<LED_1)
#define LED_OFF     PORTC &= ~(1<<LED_1)
#define LED_TOGGLE  PORTC ^= (1<<LED_1)
#else // if USE_LED_TX_RX is not defined
#define LED_DDR     
#define LED_PORT    
#define LED_1       
#define LED_ON      
#define LED_OFF     
#define LED_TOGGLE  
#endif

#endif // _LED_H_