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

#define USE_LED_TX_RX
 
#include "cRF24.h"
#include "spi.h"
#include "nRF24L01.h"
#ifdef USE_LED_TX_RX
#include "led.h"
#endif
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
    rfSetToTransmitMode();
    CE_HIGH;
    _delay_us(10);
    CE_LOW;
	LED_ON;
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

// changes the state of the transceiver to standby mode one
void rfToStandbyOne(void)
{
    rfWriteRegChar(CONFIG, SET_BIT(rfReadRegChar(CONFIG), PWR_UP));
    CE_LOW;

    _delay_ms(200);
}

// changes the state of the transceiver to powerDown mode
void rfToPowerDown(void)
{
    rfWriteRegChar(CONFIG, CLEAR_BIT(rfReadRegChar(CONFIG), PWR_UP));
    CE_LOW;
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

// writes the rfAckPayload from local buffer
uint8_t rfWriteAckPayload(const uint8_t* buf, uint8_t len)
{
    uint8_t status;

    CSN_LOW;
    status = spiSendData( W_ACK_PAYLOAD );
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
    rfToStandbyOne();
    rfSetToReceiveMode();
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

// sets the radio frequency
void rfSetChannel(uint8_t channel)
{
    // check bounds
    rfWriteRegChar(RF_CH, SET_VALUE(rfReadRegChar(RF_CH), channel));
}

// sets the transmit power
void rfSetTransmitPower(uint8_t power)
{
    // check bounds
    rfWriteRegChar(RF_SETUP, SET_VALUE(rfReadRegChar(RF_SETUP), (power<<1)));
}

// sets the transmit data-rate
void rfSetDataRate(uint8_t dataRate)
{
    // check bounds
    rfWriteRegChar(RF_SETUP, SET_VALUE(rfReadRegChar(RF_SETUP), (((1<<3) & dataRate<<3)) | ((1<<5) & dataRate<<5)));
}

// sets receive address width
void rfSetAddressWidth(uint8_t addressWidth)
{
    // check bounds
    rfWriteRegChar(SETUP_AW, SET_VALUE(rfReadRegChar(SETUP_AW), addressWidth));
}

/** \brief Enables CRC
 *
 * \return NonReturning
 *
 */
void rfEnableCRC(void)
{
    rfWriteRegChar(CONFIG, SET_BIT(rfReadRegChar(CONFIG), EN_CRC));
}

/** \brief Disables CRC
 *
 * \return NonReturning
 *
 */
void rfDisableCRC(void)
{
    rfWriteRegChar(CONFIG, CLEAR_BIT(rfReadRegChar(CONFIG), EN_CRC));
}

// sets the number of crc bytes
void rfSetCrcScheme(uint8_t scheme)
{
    if (scheme)
    {
        rfWriteRegChar(CONFIG, SET_BIT(rfReadRegChar(CONFIG), CRCO));
    }
}

// sets interrupt mask
void rfSetInterruptMask(uint8_t mask)
{
    if (MASK_RX_DR == mask)
    {
        rfWriteRegChar(CONFIG, SET_BIT(rfReadRegChar(CONFIG), MASK_RX_DR));
    }
    else if (MASK_TX_DS == mask)
    {
        rfWriteRegChar(CONFIG, SET_BIT(rfReadRegChar(CONFIG), MASK_TX_DS));
    }
    else if (MASK_MAX_RT == mask)
    {
        rfWriteRegChar(CONFIG, SET_BIT(rfReadRegChar(CONFIG), MASK_MAX_RT));
    }
    else
    {
        // something is wrong
    }
}

// clears interrupt mask
void rfClearInterruptMask(uint8_t mask)
{
    if (MASK_RX_DR == mask)
    {
        rfWriteRegChar(CONFIG, CLEAR_BIT(rfReadRegChar(CONFIG), MASK_RX_DR));
    }
    else if (MASK_TX_DS == mask)
    {
        rfWriteRegChar(CONFIG, CLEAR_BIT(rfReadRegChar(CONFIG), MASK_TX_DS));
    }
    else if (MASK_MAX_RT == mask)
    {
        rfWriteRegChar(CONFIG, CLEAR_BIT(rfReadRegChar(CONFIG), MASK_MAX_RT));
    }
    else
    {
        // something is wrong
    }
}

// sets the radio to transmit mode
void rfSetToTransmitMode(void)
{
    CE_LOW;
    rfWriteRegChar(CONFIG, CLEAR_BIT(rfReadRegChar(CONFIG), PRIM_RX));
    _delay_us(130);
}

// sets the radio to receive mode
void rfSetToReceiveMode(void)
{
    CE_LOW;
    rfWriteRegChar(CONFIG, SET_BIT(rfReadRegChar(CONFIG), PRIM_RX));
    _delay_us(130);
    CE_HIGH;
}

// sets AutoAck on a pipe
void rfSetAutoAckForPipe(uint8_t pipeAddress)
{
    if (ENAA_P0 == pipeAddress)
    {
        rfWriteRegChar(EN_AA, SET_BIT(rfReadRegChar(EN_AA), ENAA_P0));
    }
    else if (ENAA_P1 == pipeAddress)
    {
        rfWriteRegChar(EN_AA, SET_BIT(rfReadRegChar(EN_AA), ENAA_P1));
    }
    else if (ENAA_P2 == pipeAddress)
    {
        rfWriteRegChar(EN_AA, SET_BIT(rfReadRegChar(EN_AA), ENAA_P2));
    }
    else if (ENAA_P3 == pipeAddress)
    {
        rfWriteRegChar(EN_AA, SET_BIT(rfReadRegChar(EN_AA), ENAA_P3));
    }
    else if (ENAA_P4 == pipeAddress)
    {
        rfWriteRegChar(EN_AA, SET_BIT(rfReadRegChar(EN_AA), ENAA_P4));
    }
    else if (ENAA_P5 == pipeAddress)
    {
        rfWriteRegChar(EN_AA, SET_BIT(rfReadRegChar(EN_AA), ENAA_P5));
    }
    else
    {
        // something wrong
    }
}

// Clears autoack on a pipe
void rfClearAutoAckForPipe(uint8_t pipeAddress)
{
    if (ENAA_P0 == pipeAddress)
    {
        rfWriteRegChar(EN_AA, CLEAR_BIT(rfReadRegChar(EN_AA), ENAA_P0));
    }
    else if (ENAA_P1 == pipeAddress)
    {
        rfWriteRegChar(EN_AA, CLEAR_BIT(rfReadRegChar(EN_AA), ENAA_P1));
    }
    else if (ENAA_P2 == pipeAddress)
    {
        rfWriteRegChar(EN_AA, CLEAR_BIT(rfReadRegChar(EN_AA), ENAA_P2));
    }
    else if (ENAA_P3 == pipeAddress)
    {
        rfWriteRegChar(EN_AA, CLEAR_BIT(rfReadRegChar(EN_AA), ENAA_P3));
    }
    else if (ENAA_P4 == pipeAddress)
    {
        rfWriteRegChar(EN_AA, CLEAR_BIT(rfReadRegChar(EN_AA), ENAA_P4));
    }
    else if (ENAA_P5 == pipeAddress)
    {
        rfWriteRegChar(EN_AA, CLEAR_BIT(rfReadRegChar(EN_AA), ENAA_P5));
    }
    else
    {
        // something wrong
    }
}

// enables an address pipe
void rfEnablePipeForRx(uint8_t pipeAddress)
{
    if (ERX_P0 == pipeAddress)
    {
        rfWriteRegChar(EN_RXADDR, SET_BIT(rfReadRegChar(EN_RXADDR), ERX_P0));
    }
    else if (ERX_P1 == pipeAddress)
    {
        rfWriteRegChar(EN_RXADDR, SET_BIT(rfReadRegChar(EN_RXADDR), ERX_P1));
    }
    else if (ERX_P2 == pipeAddress)
    {
        rfWriteRegChar(EN_RXADDR, SET_BIT(rfReadRegChar(EN_RXADDR), ERX_P2));
    }
    else if (ERX_P3 == pipeAddress)
    {
        rfWriteRegChar(EN_RXADDR, SET_BIT(rfReadRegChar(EN_RXADDR), ERX_P3));
    }
    else if (ERX_P4 == pipeAddress)
    {
        rfWriteRegChar(EN_RXADDR, SET_BIT(rfReadRegChar(EN_RXADDR), ERX_P4));
    }
    else if (ERX_P5 == pipeAddress)
    {
        rfWriteRegChar(EN_RXADDR, SET_BIT(rfReadRegChar(EN_RXADDR), ERX_P5));
    }
    else
    {
        // something wrong
    }
}

// disable an address pipe
void rfDisablePipeForRx(uint8_t pipeAddress)
{
    if (ERX_P0 == pipeAddress)
    {
        rfWriteRegChar(EN_RXADDR, CLEAR_BIT(rfReadRegChar(EN_RXADDR), ERX_P0));
    }
    else if (ERX_P1 == pipeAddress)
    {
        rfWriteRegChar(EN_RXADDR, CLEAR_BIT(rfReadRegChar(EN_RXADDR), ERX_P1));
    }
    else if (ERX_P2 == pipeAddress)
    {
        rfWriteRegChar(EN_RXADDR, CLEAR_BIT(rfReadRegChar(EN_RXADDR), ERX_P2));
    }
    else if (ERX_P3 == pipeAddress)
    {
        rfWriteRegChar(EN_RXADDR, CLEAR_BIT(rfReadRegChar(EN_RXADDR), ERX_P3));
    }
    else if (ERX_P4 == pipeAddress)
    {
        rfWriteRegChar(EN_RXADDR, CLEAR_BIT(rfReadRegChar(EN_RXADDR), ERX_P4));
    }
    else if (ERX_P5 == pipeAddress)
    {
        rfWriteRegChar(EN_RXADDR, CLEAR_BIT(rfReadRegChar(EN_RXADDR), ERX_P5));
    }
    else
    {
        // something wrong
    }
}

// set number of retries
void rfSetRetryAttempts(uint8_t attempts)
{
    // check for bounds
    rfWriteRegChar(SETUP_RETR, SET_VALUE(rfReadRegChar(SETUP_RETR), attempts));
}

// sets the retransmit delays
void rfSetRetryDelay(uint8_t delay)
{
    // check for bounds
    rfWriteRegChar(SETUP_RETR, SET_VALUE(rfReadRegChar(SETUP_RETR), (delay<<4)));
}

// make the channel busy by transmitting carrier wave
void rfTransmitCarrier(void)
{
    rfWriteRegChar(RF_SETUP, SET_BIT(rfReadRegChar(RF_SETUP), CONT_WAVE));
}

// reads the channel power
uint8_t rfReadChannelPower(void)
{
    return rfReadRegChar(RPD);
}

// enables dynamic payload for a pipe
void rfSetDynamicPayloadForPipe(uint8_t dymamic)
{
    if (DPL_P0 == dymamic)
    {
        rfWriteRegChar(DYNPD, SET_BIT(rfReadRegChar(DYNPD), DPL_P0));
    }
    else if (DPL_P1 == dymamic)
    {
        rfWriteRegChar(DYNPD, SET_BIT(rfReadRegChar(DYNPD), DPL_P1));
    }
    else if (DPL_P2 == dymamic)
    {
        rfWriteRegChar(DYNPD, SET_BIT(rfReadRegChar(DYNPD), DPL_P2));
    }
    else if (DPL_P3 == dymamic)
    {
        rfWriteRegChar(DYNPD, SET_BIT(rfReadRegChar(DYNPD), DPL_P3));
    }
    else if (DPL_P4 == dymamic)
    {
        rfWriteRegChar(DYNPD, SET_BIT(rfReadRegChar(DYNPD), DPL_P4));
    }
    else if (DPL_P5 == dymamic)
    {
        rfWriteRegChar(DYNPD, SET_BIT(rfReadRegChar(DYNPD), DPL_P5));
    }
    else
    {
        // something wrong
    }
}

// enables dynamic payload for a pipe
void rfClearDynamicPayloadForPipe(uint8_t dymamic)
{
    if (DPL_P0 == dymamic)
    {
        rfWriteRegChar(DYNPD, CLEAR_BIT(rfReadRegChar(DYNPD), DPL_P0));
    }
    else if (DPL_P1 == dymamic)
    {
        rfWriteRegChar(DYNPD, CLEAR_BIT(rfReadRegChar(DYNPD), DPL_P1));
    }
    else if (DPL_P2 == dymamic)
    {
        rfWriteRegChar(DYNPD, CLEAR_BIT(rfReadRegChar(DYNPD), DPL_P2));
    }
    else if (DPL_P3 == dymamic)
    {
        rfWriteRegChar(DYNPD, CLEAR_BIT(rfReadRegChar(DYNPD), DPL_P3));
    }
    else if (DPL_P4 == dymamic)
    {
        rfWriteRegChar(DYNPD, CLEAR_BIT(rfReadRegChar(DYNPD), DPL_P4));
    }
    else if (DPL_P5 == dymamic)
    {
        rfWriteRegChar(DYNPD, CLEAR_BIT(rfReadRegChar(DYNPD), DPL_P5));
    }
    else
    {
        // something wrong
    }
}

// enable dynamic payload
void rfEnableGlobalDynamicPayload(void)
{
    rfWriteRegChar(FEATURE, SET_BIT(rfReadRegChar(FEATURE), EN_DPL));
}

// enable dynamic payload
void rfDisableGlobalDynamicPayload(void)
{
    rfWriteRegChar(FEATURE, CLEAR_BIT(rfReadRegChar(FEATURE), EN_DPL));
}

// enable Ack payload
void rfEnableGlobalAckPayload(void)
{
    rfWriteRegChar(FEATURE, SET_BIT(rfReadRegChar(FEATURE), EN_ACK_PAY));
}

// disable Ack payload
void rfDisableGlobalAckPayload(void)
{
    rfWriteRegChar(FEATURE, CLEAR_BIT(rfReadRegChar(FEATURE), EN_ACK_PAY));
}

// enables dynamic payload
void rfEnableGlobalDynamicAckPayload(void)
{
    rfWriteRegChar(FEATURE, SET_BIT(rfReadRegChar(FEATURE), EN_DYN_ACK));
}

// disable dynamic payload
void rfDisableGlobalDynamicAckPayload(void)
{
    rfWriteRegChar(FEATURE, CLEAR_BIT(rfReadRegChar(FEATURE), EN_DYN_ACK));
}

// status about the TX FIFO status
uint8_t rfTxFifoStatus(void)
{
    uint8_t status = rfReadRegChar(FIFO_STATUS);
    return (CHECK_BIT(status,TX_FULL));
}

// status about the RX FIFO status
uint8_t rfRxFifoStatus(void)
{
    uint8_t status = rfReadRegChar(FIFO_STATUS);
    return (CHECK_BIT(status,RX_FULL));
}

/** \brief
 *
 *
 */
ISR(EXT_INT)
{
    uint8_t status;
    status = rfStatus();

    if(CHECK_BIT(status, RX_DR))
    {
        SET_BIT(status, RX_DR);
		LED_ON;
        rfReceived();
        uint8_t length = rfReadRegChar(R_RX_PL_WID);
        rfReadPayload(length);
		LED_OFF;
    }

    if(CHECK_BIT(status, TX_DS))
    {
        SET_BIT(status, TX_DS);
		LED_OFF;
        rfTransmited();
    }

    if(CHECK_BIT(status, MAX_RT))
    {
        SET_BIT(status, MAX_RT);
		// packet transmit failed
    }
}
