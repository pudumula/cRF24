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

#ifndef _SPI_H_
#define _SPI_H_

#include <inttypes.h>

#define PORT_SET(PORT,BIT)   ((PORT) |=  (1<<(BIT)))
#define PORT_RESET(PORT,BIT) ((PORT) &= ~(1<<(BIT)))
#define TOGGLE_BIT(PORT,BIT) ((PORT) ^=  (1<<(BIT)))
#define DDR_SET(PORT,BIT)    ((PORT) |=  (1<<(BIT)))
#define DDR_RESET(PORT,BIT)  ((PORT) &= ~(1<<(BIT)))
#define DDR_BIT(PORT,BIT)    ((PORT) ^=  (1<<(BIT)))

#define MOSI     3
#define MISO     4
#define SCK      5
#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define SPI_PIN  PINB

#define CSN      7
#define CSN_DDR  DDRB
#define CSN_PORT PORTB
#define CSN_OUT  DDR_SET(CSN_DDR,CSN)
#define CSN_IN   DDR_RESET(CSN_DDR,CSN)
#define CSN_H    PORT_SET(CSN_PORT,CSN)
#define CSN_L    PORT_RESET(CSN_PORT,CSN)

#define CE      6
#define CE_DDR  DDRB
#define CE_PORT PORTB
#define CE_OUT  DDR_SET(CE_DDR,CE)
#define CE_IN   DDR_RESET(CE_DDR,CE)
#define CE_H    PORT_SET(CE_PORT,CE)
#define CE_L    PORT_RESET(CE_PORT,CE)

#define SPI_SEND  loop_until_bit_is_set(SPSR, SPIF)
#define SPI_DATA  SPDR

// initializing spi
void spiInit(void);

// sending the data stored in the buffer.
void spiSendBuffer(uint8_t *buffer, uint8_t len);

// gets the data from the radio and stores in the buffer
void spiGetBuffer(uint8_t *buffer, uint8_t len);

// sends any command
uint8_t spiSendData(uint8_t data);

#endif // _SPI_H_
