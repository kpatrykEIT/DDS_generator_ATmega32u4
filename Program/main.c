/*
* File: main.c 
*
* Created: 31.05.2025 11:22:55
* Author : Patryk Krukowski 
*/

#include "main.h"

// Zmienne dla LCD
// 1 - Sinusoidalny
// 2 - Prostokatny
// 3 - Trojkatny
// 4 - Piloksztaltny

volatile uint8_t mode = 1;
volatile bool output = false;

// Zmienne dla DDS

volatile int32_t DDS_fout = 100; 
volatile uint32_t DDS_phaseACC = 0; 
volatile uint8_t DDS_index = 0;
volatile uint32_t DDS_tunningWord = 0;
uint32_t DDS_fsample = 100000; // Hz
const uint8_t DDS_preskaler = 8;

int main(void)
{	
	avr_init();
	timer1_init();
	
	// LCD inicjalizacja
	
	i2c_master_init(I2C_SCL_FREQUENCY_400);
	lq_init(0x27, 20, 4, LCD_5x8DOTS);
	LiquidCrystalDevice_t device = lq_init(0x27, 20, 4, LCD_5x8DOTS);
	lq_turnOnBacklight(&device);
	display_author_info(device);
	//_delay_ms(2000);
	//lq_clear(&device);
	
	sei();
	while(1){
		
		if(output)
		{
			PORTF |= (1 << LED_GREEN);
			PORTF &= ~(1 << LED_RED);
			TIMSK1 |= (1 << OCIE1A); // wlacz przerwania Timera1
			
		}
		if(output == false)
		{
			PORTF |= (1 << LED_RED);
			PORTF &= ~(1 << LED_GREEN);
			TIMSK1 &= ~(1 << OCIE1A); // wylacz przerwania Timera1
			
			// Zmiana wyjsciowej czestotliwosci
			if(BTN_PRESSED(PINF, BTN_UP))
			{
				DDS_fout += 100; // + 100 Hz
				if(DDS_fout > 15000)
				{
					DDS_fout = 15000;
				}
				//_delay_ms(50);
				
			}
			if(BTN_PRESSED(PINF, BTN_DOWN))
			{
				DDS_fout -= 100; // - 100 Hz		
				if(DDS_fout < 0)
				{
					DDS_fout = 0;
				}
				//_delay_ms(50);
			}
			DDS_tunningWord = (pow(2,32)*DDS_fout)/DDS_fsample;
			// Zmiana ksztaltu przebiegu
			if(BTN_PRESSED(PINF,BTN_MODE) && output == false)
			{
				mode ++;
				if(mode > 4)
				{
					mode = 1;
				}
			}
		}
		if(BTN_PRESSED(PINF, BTN_OUTPUT))
		{
			output = false;
		}
		if(!BTN_PRESSED(PINF, BTN_OUTPUT))
		{
			output = true;
		}
		main_menu(device, mode, output, DDS_fout);
	}
}

// Wysylanie probki do DACa

ISR(TIMER1_COMPA_vect)
{
	if(BTN_PRESSED(PINF, BTN_OUTPUT))
	{
		TIMSK1 &= ~(1 << OCIE1A); // wylacz przerwania Timera1
	}
	DDS_phaseACC += DDS_tunningWord;
	DDS_index = (DDS_phaseACC >> 24);
	if(mode == 1)
	{
		 DAC_OUTPUT = sin_wave256[DDS_index];
	}
	if(mode == 2)
	{
		DAC_OUTPUT = square_wave256[DDS_index];
	 
	}
	if(mode == 3)
	{
		DAC_OUTPUT = triangle_wave256[DDS_index];
	} 
	if(mode == 4)
	{
		 DAC_OUTPUT = sawtooth_wave256[DDS_index];
	}
}
inline void timer1_init(void)
{
	TCCR1B |= (1 << WGM12) | (1 << CS11); // CTC, 8 preskaler
	OCR1A  = (F_CPU/(DDS_preskaler*DDS_fsample)) - 1;
}

	
// Inicjalizacja wejsc/wyjsc
	
inline void avr_init(void)
{
	// Wylacz przerwania USB
	UDIEN = 0;
	// Przyciski jako wejscia
	DDRF &= ~(1 << BTN_MODE);
	DDRF &= ~(1 << BTN_UP);
	DDRF &= ~(1 << BTN_DOWN);
	DDRF &= ~(1 << BTN_OUTPUT);
	// PORTB jako wyjscia dla C/A
	DDRB |= (1 << DAC_DB0);
	DDRB |= (1 << DAC_DB1);
	DDRB |= (1 << DAC_DB2);
	DDRB |= (1 << DAC_DB3);
	DDRB |= (1 << DAC_DB4);
	DDRB |= (1 << DAC_DB5);
	DDRB |= (1 << DAC_DB6);
	DDRB |= (1 << DAC_DB7);
	DDRD |= (1 << DAC_WR);
}

// Informacje o autorze

void display_author_info(struct LiquidCrystalDevice_t device)
{
	lq_setCursor(&device, 0 , 2);
	lq_print(&device, "PRACA DYPLOMOWA");
	lq_setCursor(&device, 1 , 4);
	lq_print(&device, "INZYNIERSKA");
	lq_setCursor(&device, 2 , 0);
	lq_print(&device, "Patryk Krukowski EiT");
	lq_setCursor(&device, 3 , 4);
	lq_print(&device, "GDANSK, 2026");
	_delay_ms(3000);
	lq_clear(&device);
}

// Menu glowne

void main_menu(struct LiquidCrystalDevice_t device, uint8_t mode, bool output, int32_t fout)
{
	char fout_buffer[32];
	sprintf(fout_buffer,"%-5lu",fout);
	lq_setCursor(&device,0,0);
	lq_print(&device,"********************");
	lq_setCursor(&device, 1, 0);
	lq_print(&device, "MODE: ");
	lq_setCursor(&device, 2, 0);
	lq_print(&device, "FREQUENCY: ");
	lq_setCursor(&device, 2, 11);
	lq_print(&device, fout_buffer);
	lq_setCursor(&device, 2, 18);
	lq_print(&device, "Hz");
	lq_setCursor(&device, 3,0);
	lq_print(&device, "OUTPUT: ");
	if(output)
	{
		lq_setCursor(&device,3,8);
		lq_print(&device, "ON ");
	}
	if(output == false)
	{
		lq_setCursor(&device,3,8);
		lq_print(&device, "OFF ");
	}
	if(mode == 1)
	{
		lq_setCursor(&device, 1, 6);
		lq_print(&device, "SINE      ");
		lq_turnOnBlink(&device);
		_delay_ms(100);
		lq_turnOffBlink(&device);
		//lq_setCursor(&device, 1, 10);
		//lq_print(&device,"     ");
	}
	if(mode == 2)
	{
		lq_setCursor(&device, 1, 6);
		lq_print(&device, "SQUARE    ");
		lq_turnOnBlink(&device);
		_delay_ms(100);
		lq_turnOffBlink(&device);
	}
	if(mode == 3)
	{
		lq_setCursor(&device, 1, 6);
		lq_print(&device, "TRIANGLE    ");
		lq_turnOnBlink(&device);
		_delay_ms(100);
		lq_turnOffBlink(&device);
	}
	if(mode == 4)
	{
		lq_setCursor(&device, 1, 6);
		lq_print(&device, "SAWTOOTH   ");
		lq_turnOnBlink(&device);
		_delay_ms(100);
		lq_turnOffBlink(&device);
	}
}

