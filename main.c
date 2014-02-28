#include <avr/io.h>
#include "main.h"
#include "cRF24.h"
#include "spi.h"
#include "nRF24L01.h"
#include "led.h"

#define RF_CHANNEL 26

void init(void)
{
	rfToStandbyOne();
	rfSetChannel(RF_CHANNEL);
	rfSetInterruptMask(MASK_RX_DR | MASK_TX_DS | MASK_MAX_RT);
	rfSetAutoAckForPipe(ENAA_P0);
	rfEnablePipeForRx(ERX_P0);
	rfEnableGlobalDynamicPayload();
	rfSetDynamicPayloadForPipe(DPL_P0);
	rfStartListening();
}

int main(void)
{
    init();
	while(1)
	{
		
	}
    return 0;
}
