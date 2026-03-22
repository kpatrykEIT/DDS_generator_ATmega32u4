/*
 * File: main.h
 *
 * Created: 02.06.2025 17:09:30
 *  Author: Patryk Krukowski
 */ 


#ifndef MAIN_H_
#define MAIN_H_

// BIBLIOTEKI

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "sin_wave.h"
#include "sawtooth_wave.h"
#include "square_wave.h"
#include "triangle_wave.h"
#include "i2c_master.h"
#include "liquid_crystal_i2c.h"

// PRZYCISKI

#define BTN_MODE	PF4		// zmien ksztalt przebiegu
#define BTN_UP		PF5		// zwieksz czestotliwosc
#define BTN_DOWN	PF6		// zmniejsz czestotliwosc
#define BTN_OUTPUT	PF7		// wlacz/wylacz wyjscie generatora

// PRZETWORNIK C/A


#define DAC_WR	PD6
#define DAC_DB0 PB0
#define DAC_DB1 PB1
#define DAC_DB2 PB2
#define DAC_DB3 PB3
#define DAC_DB4 PB4
#define DAC_DB5 PB5
#define DAC_DB6 PB6
#define DAC_DB7 PB7
#define DAC_OUTPUT	PORTB


// MAKRA

#define BTN_PRESSED(PIN,BIT)	(!((PIN) & (1 << (BIT))))

// FUNKCJE

inline void avr_init(void);
void display_author_info(struct LiquidCrystalDevice_t device);
void main_menu(struct LiquidCrystalDevice_t device, uint8_t mode, bool output, int32_t fout);
inline void timer1_init(void);

#endif /* MAIN_H_ */