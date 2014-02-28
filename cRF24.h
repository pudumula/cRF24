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

// INT0 external vector
#define EXT_INT INT0_vect
// INT1 external vector
//#define EXT_INT INT1_vect

#define CSN_HIGH CSN_H
#define CSN_LOW  CSN_L
#define CE_HIGH  CE_H
#define CE_LOW   CE_L

#define CHECK_BIT(VAL,MASK)      (VAL & (1<<MASK))
#define SET_BIT(VAL,MASK)        (VAL | (1<<MASK))
#define CLEAR_BIT(VAL,MASK)      (VAL & ~(1<<MASK))

#define CHECK_VALUE(VAL,MASK)    (VAL & (MASK))
#define SET_VALUE(VAL,MASK)      (VAL | (MASK))
#define CLEAR_VALUE(VAL,MASK)    (VAL & ~(MASK))

typedef enum
{
	RX_MODE,
	RX_LISTENING,
	RX_DATA,
	TX_MODE,
	TX_TRANSMITTING,
	TX_TRANSMITTED,
	TX_RETRANSMIT_FAIL,
	IDLE
} RFstate_t;

// handles the transmission of rfPacket
void rfTransmit(uint8_t* buff, uint8_t len);

// handles the reception of rfPacket and filling of the buffer
void rfReceived(void);

// handles the confirmation of transmission
void rfTransmited(void);

// changes the state of the transceiver to standby mode one
void rfToStandbyOne(void);

// changes the state of the transceiver to powerDown mode
void rfToPowerDown(void);

// continuously listens
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

// flush rx FIFO
uint8_t rfFlushRx(void);

// flush TX FIFO
uint8_t rfFlushTx(void);

// rf status
uint8_t rfStatus(void);

// sets the radio frequency
void rfSetChannel(uint8_t channel);

// sets the transmit power
void rfSetTransmitPower(uint8_t power);

// sets the transmit data-rate
void rfSetDataRate(uint8_t dataRate);

// sets receive address with
void rfSetAddressWidth(uint8_t addressWidth);

/** \brief Enables CRC
 *
 * \return NonReturning
 *
 */
void rfEnableCRC(void);

/** \brief Disables CRC
 *
 * \return NonReturning
 *
 */
void rfDisableCRC(void);

// sets the number of crc bytes
void rfSetCrcScheme(uint8_t scheme);

// sets interrupt mask
void rfSetInterruptMask(uint8_t mask);

// clears interrupt mask
void rfClearInterruptMask(uint8_t mask);

// sets the radio to transmit mode
void rfSetToTransmitMode(void);

// sets the radio to receive mode
void rfSetToReceiveMode(void);

// sets AutoAck on a pipe
void rfSetAutoAckForPipe(uint8_t pipeAddress);

// Clears autoack on a pipe
void rfClearAutoAckForPipe(uint8_t pipeAddress);

// enables an address pipe
void rfEnablePipeForRx(uint8_t pipeAddress);

// disable an address pipe
void rfDisablePipeForRx(uint8_t pipeAddress);

// set number of retries
void rfSetRetryAttempts(uint8_t attempts);

// sets the retransmit delays
void rfSetRetryDelay(uint8_t delay);

// make the channel busy by transmitting carrier wave
void rfTransmitCarrier(void);

// reads the channel power
uint8_t rfReadChannelPower(void);

// enables dynamic payload for a pipe
void rfSetDynamicPayloadForPipe(uint8_t dymamic);

// enables dynamic payload for a pipe
void rfClearDynamicPayloadForPipe(uint8_t dymamic);

// enable dynamic payload
void rfEnableGlobalDynamicPayload(void);

// disable dynamic payload
void rfDisableGlobalDynamicPayload(void);

// enable Ack payload
void rfEnableGlobalAckPayload(void);

// disable Ack payload
void rfDisableGlobalAckPayload(void);

// enables dynamic payload
void rfEnableGlobalDynamicAckPayload(void);

// disable dynamic payload
void rfDisableGlobalDynamicAckPayload(void);

// status about the TX FIFO status
uint8_t rfTxFifoStatus(void);

// status about the RX FIFO status
uint8_t rfRxFifoStatus(void);

// writes the rfAckPayload from local buffer
uint8_t rfWriteAckPayload(const uint8_t* buf, uint8_t len);

#endif // _CRF24_H_
