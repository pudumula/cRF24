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

#include "cRF24.h"
#include "spi.h"
#include "nRF24L01.h"
#include <util/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>

volatile uint8_t receivedFlag = 0;
volatile uint8_t transmittedFlag = 0;
volatile uint8_t rfBuffer[32];
volatile uint8_t *rfPtrBuffer = rfBuffer;

// handles the transmission of rfPacket
void rfTransmit(uint8_t* buff, uint8_t len)
{
    rfToStandbyOne();
    rfWritePayload(buff, len);
    uint8_t result = rfReadRegChar(CONFIG);
    rfWriteRegChar(CONFIG, CLEAR_BIT(result, PRIM_RX));
    CE_HIGH;
    _delay_us(10);
    CE_LOW;
}

// handles the reception of rfPacket and filling of the buffer
void rfReceived(void)
{
    // check if the receivedFlag is one if so raise assert
    receivedFlag = 1;
}

// handles the confirmation of transmission
void rfTransmited(void)
{
    // check if the transmittedFlag is one if so raise assert
    transmittedFlag = 1;
}

// changes the state of the tranciver to standby mode one
void rfToStandbyOne(void)
{
    rfWriteRegChar(CONFIG, SET_BIT(rfReadRegChar(CONFIG), PWR_UP));

    _delay_ms(200);
}

// changes the state of the tranciver to powerDown mode
void rfToPowerDown(void)
{
    rfWriteRegChar(CONFIG, CLEAR_BIT(rfReadRegChar(CONFIG), PWR_UP));
}

// reads the register and writes to the buffer
uint8_t rfReadRegBuff(uint8_t reg, uint8_t* buf, uint8_t len)
{
    uint8_t status;

    CSN_LOW;
    status = spiSendData( R_REGISTER | reg );
    while ( len-- )
        *buf++ = spiSendData(0xff);
    CSN_HIGH;

    return status;
}

// reads the register and returns it
uint8_t rfReadRegChar(uint8_t reg)
{
    CSN_LOW;
    spiSendData( R_REGISTER | reg );
    uint8_t result = spiSendData(0xff);
    CSN_HIGH;

    return result;
}

// writes to the register from buffer
uint8_t rfWriteRegBuff(uint8_t reg, const uint8_t* buf, uint8_t len)
{
    uint8_t status;

    CSN_LOW;
    status = spiSendData( W_REGISTER | reg );
    while ( len-- )
        spiSendData(*buf++);
    CSN_HIGH;

    return status;
}

// writes to the register
uint8_t rfWriteRegChar(uint8_t reg, uint8_t value)
{
    uint8_t status;

    CSN_LOW;
    status = spiSendData( W_REGISTER | reg );
    spiSendData(value);
    CSN_HIGH;

    return status;
}

// writes the rfPayload from local buffer
uint8_t rfWritePayload(const uint8_t* buf, uint8_t len)
{
    uint8_t status;

    CSN_LOW;
    status = spiSendData( W_TX_PAYLOAD );
    while ( len-- )
        spiSendData(*buf++);
    CSN_HIGH;

    return status;
}

// reads the rfPayload into local buffer
uint8_t rfReadPayload(uint8_t len)
{
    uint8_t status;

    CSN_LOW;
    status = spiSendData( R_RX_PAYLOAD );
    while ( len-- )
        *rfPtrBuffer++ = spiSendData(NOP);
    CSN_HIGH;

    return status;
}

// continuously listens
void rfStartListening(void)
{
    CE_LOW;
    rfWriteRegChar(CONFIG, SET_BIT(rfReadRegChar(CONFIG), PRIM_RX));
    _delay_us(130);
    CE_HIGH;
}

// stops listening
void rfStopListening(void)
{
    CE_LOW;
    rfFlushTx();
    rfFlushRx();
}

// flush rx FIFO
uint8_t rfFlushRx(void)
{
    uint8_t status;

    CSN_LOW;
    status = spiSendData( FLUSH_RX );
    CSN_HIGH;

    return status;
}

// flush TX FIFO
uint8_t rfFlushTx(void)
{
    uint8_t status;

    CSN_LOW;
    status = spiSendData( FLUSH_TX );
    CSN_HIGH;

    return status;
}

// rf status
uint8_t rfStatus(void)
{
    uint8_t status;

    CSN_LOW;
    status = spiSendData(NOP);
    CSN_HIGH;

    return status;
}

ISR(INT0_vect)
{
    uint8_t status;
    status = rfStatus();

    if(CHECK_BIT(status, RX_DR))
    {
        rfReceived();
        rfReadPayload(32);
    }

    if(CHECK_BIT(status, TX_DS))
    {
        rfTransmited();
    }
}
