#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "cRF24.h"
#include "spi.h"
#include "nRF24L01.h"
#include "led.h"

#define RF_CHANNEL 26

extern rfState_t rfState;
extern uint8_t rfLength;
extern uint8_t rfPtrBuffer;



void init(void)
{
	// move from power down to standby mode one
	rfToStandbyOne();
	// set the channel/frequency
	rfSetChannel(RF_CHANNEL);
	// set the mask bits. MASK_MAX_RT is set because we use auto ack
	rfSetInterruptMask(MASK_RX_DR | MASK_TX_DS | MASK_MAX_RT);
	// enable auto ack for pipe P0
	rfSetAutoAckForPipe(ENAA_P0);
	// enable RX for pipe P0
	rfEnablePipeForRx(ERX_P0);
	// enable reception of dynamic payload
	rfEnableGlobalDynamicPayload();
	// enable dynamic payload for pipe P0
	rfSetDynamicPayloadForPipe(DPL_P0);
	// enable listening
	rfStartListening();
	// enable global interrupts
	sei();
}

int main(void)
{
    init();
	while(1)
	{
	    if (rfState == RF_DATA_RECEIVED)
        {
            app(APP_DATA_RECEIVED);
        }
        else if (rfState == RF_LISTENING)
        {
            app(APP_RUN);
        }
	}
    return 0;
}

void app(appState_t appState)
{
    if (appState == APP_DATA_RECEIVED)
    {
        processData();
    }
    // write app here
}

void processData(void)
{
    //uint8_t appBuffer[32];

}

void appTransmit(uint8_t *data, uint8_t len)
{
    rfTransmit(data,len);
    while(rfState != RF_TRANSMIT_SUCCESS);
    rfStartListening();
}
