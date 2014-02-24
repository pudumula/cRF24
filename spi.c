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

#include <avr/io.h>
#include <avr/io.h>
#include "spi.h"

// initializing spi
void spiInit(void)
{
    // enabling spi
    SPCR |= (1<<SPE);
    // selecting master mode
    SPCR |= (1<<MSTR);
    // making pins output, the miso pin will be made input when spi is enabled
    SPI_DDR |= (1<<MOSI) | (1<<SCK);
}

// sending the data stored in the buffer.
void spiSendBuffer(uint8_t *buffer, uint8_t len)
{
    do
    {
        SPI_DATA = *buffer;
        SPI_SEND;
        buffer++;
    }
    while(len--);
}

// gets the data from the radio and stores in the buffer
void spiGetBuffer(uint8_t *buffer, uint8_t len)
{
    do
    {
        SPI_DATA = 0xff;
        SPI_SEND;
        *buffer = SPI_DATA;
        buffer++;
    }
    while(len--);
}

// sends any command
uint8_t spiSendData(uint8_t data)
{
    uint8_t result;

    SPI_DATA = data;
    SPI_SEND;
    result = SPI_DATA;
    // returns the status
    return result;
}

