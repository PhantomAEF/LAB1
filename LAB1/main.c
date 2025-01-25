/*
 * LAB1.c
 *
 * Created: 23/01/2025 19:45:53
 * Author : alane
 */ 
#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "SEGLIB/SEGLIB.h"
void setup(void);
void startRace(void);

int count = 0;

void startRace() {
	for (int i = 5; i >= 0; i--) {
		displayref(i);
		_delay_ms(1000);
	}
	PORTD = 0b11111111; // Apagar display después de conteo
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
	     PORTD = 0b11111111;
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
	
}
