//******************************************************************************
// Universidad Del Valle De Guatemala
// IE2023: Electrónica Digital 2
// Autor: Alan Flores
// Carné: 22456
// Proyecto: Laboratorio 1
// Hardware: Atmega238p
// Creado: 23/01/2025
//******************************************************************************
#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "SEGLIB/SEGLIB.h"

//******************************************************************************
void setup(void);
void startRace(void);

void checkWinner(void);
void updateLeds(uint8_t player, volatile uint8_t *port, uint8_t shift);
//******************************************************************************
int count = 0;

int player1 = 0;
int player2 = 0;
int result;
int raceStarted = 0;
//******************************************************************************
void startRace() {
	for (int i = 5; i >= 0; i--) {
		displayref(i);
		_delay_ms(1000);
	}
	PORTD = 0b11111111; // Apagar display después de conteo
	raceStarted = 1; // Iniciar la carrera
	player1 = 0;
	player2 = 0;
	result = 0;
	PORTB &= 0xE1; // Limpiar los bits PB1-PB4
	PORTC &= 0xF0; // Limpiar los bits PC0-PC3
}

void setup(void) {
	cli();
	
	DDRB = 0b11011110;   // PB0 como entrada, PB1-PB4 como salidas
	PORTB = 0b00100001;  // Pull Up en PB0 y PB5
	
	DDRD = 0b11111111;   // Salida de DISPLAY
	PORTD = 0b11111111;  // DISPLAY apagado
	
	DDRC = 0b00001111;   // PC0-PC3 como salidas, PC4-PC6 como entradas
	PORTC = 0b11110000;  // Pull Up en PC4-PC6
	
	PCMSK1 |= (1<<PCINT12)|(1<<PCINT13)|(1<<PCINT14); // PCINT12 (PC4), PCINT13 (PC5), PCINT14 (PC6)
	PCICR |= (1<<PCIE1);  // Mascara de interrupción para PCINT[14:8]
	
	PCICR |= (1 << PCIE0); // Habilitar interrupciones de PCINT[7:0]
	PCMSK0 |=  (1 << PCINT5) | (1 << PCINT0); // Habilitar interrupción en  PB5 y PB0
	
	PCICR |= (1<<PCIE0); // Mascara de interrupción
	UCSR0B = 0;
	
	sei(); // Activar interrupciones
}

int main(void)
{
	setup();
     while (1) {
	     checkWinner();
     }
    
}

void checkWinner() {
	if (result != 0) {
		_delay_ms(10);
		PORTD = 0x0F;     // Apagar el display después de mostrar el ganador
	}

	switch (result) {
		case 1:
		displayref(1);
		count = 0;
		break;
		
		case 2:
		displayref(2);
		count = 0;
		break;
	}
}

void updateLeds(uint8_t player, volatile uint8_t *port, uint8_t shift) {
	uint8_t mask = 0x0F << shift;
	*port &= ~mask; // Apagar los LEDs correspondientes
	if (player > 0 && player <= 4) {
		*port |= (1 << (player - 1 + shift)); // Encender el LED específico
	}
}

ISR(PCINT1_vect) {
	if ((PINC & (1<<PINC4)) == 0) {
		count++;
		if (count > 1) {
			startRace(); // Reiniciar la carrera si ya ha iniciado
			} else {
			startRace(); // Iniciar la carrera si no ha iniciado
		}
	}
	
	 if ((PINC & (1<<PINC5)) == 0 && raceStarted) {
		 if (result != 2) { // Evitar incrementar si player2 ha ganado
			 player1++;
			 if (player1 > 4) {
				 player1 = 4;   // Limitar el valor máximo a 4
				 result = 1;
			 }
			 updateLeds(player1, &PORTB, 1); // Actualizar PB1-PB4 con el valor de player1
		 }
	 } 
	 
	 
}

ISR(PCINT0_vect) {
	if ((PINB & (1<<PINB0)) == 0 && raceStarted) {
		if (result != 1) { // Evitar incrementar si player1 ha ganado
			player2++;
			if (player2 > 4) {
				player2 = 4;   // Limitar el valor máximo a 4
				result = 2;
			}
			updateLeds(player2, &PORTC, 0); // Actualizar PC0-PC3 con el valor de player2
		}
	}
}