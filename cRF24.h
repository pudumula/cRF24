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

#ifndef _CRF24_H_
#define _CRF24_H_

#include <inttypes.h>

#define CSN_HIGH CSN_H
#define CSN_LOW  CSN_L
#define CE_HIGH  CE_H
#define CE_LOW   CE_L

#define CHECK_BIT(VAL,MASK)    (VAL & (1<<MASK))
#define SET_BIT(VAL,MASK)      (VAL | (1<<MASK))
#define CLEAR_BIT(VAL,MASK)    (VAL & ~(1<<MASK))

// handles the transmission of rfPacket
void rfTransmit(uint8_t* buff, uint8_t len);

// handles the reception of rfPacket and filling of the buffer
void rfReceived(void);

// handles the confirmation of transmission
void rfTransmited(void);

// changes the state of the tranciver to standby mode one
void rfToStandbyOne(void);

// changes the state of the tranciver to powerDown mode
void rfToPowerDown(void);

// continously listens
void rfStartListening(void);

// reads the register and writes to the buffer
uint8_t rfReadRegBuff(uint8_t reg, uint8_t* buf, uint8_t len);

// reads the register and returns it
uint8_t rfReadRegChar(uint8_t reg);

// writes to the register from buffer
uint8_t rfWriteRegBuff(uint8_t reg, const uint8_t* buf, uint8_t len);

// writes to the register
uint8_t rfWriteRegChar(uint8_t reg, uint8_t value);

// writes the rfPayload from local buffer
uint8_t rfWritePayload(const uint8_t* buf, uint8_t len);

// reads the rfPayload into local buffer
uint8_t rfReadPayload(uint8_t len);

// flush rx FIFO
uint8_t rfFlushRx(void);

// flush TX FIFO
uint8_t rfFlushTx(void);

// rf status
uint8_t rfStatus(void);

// stops listening
void rfStopListening(void);

#endif // _CRF24_H_
