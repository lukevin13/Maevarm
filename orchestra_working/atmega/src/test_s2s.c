#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include <math.h>

#define DEF_F 500
#define CHANNEL 1
#define ADDRESS 0x40
#define PACKET_LENGTH 3

volatile int rec_flag = 0;

void init();
void pwm_setup();

int main() {
	init();
	char buffer[3] = {0,0,0};
	int counter = 0;
	int duration = 0;
	int state = 0;

	while(1) {

		if (rec_flag) {
			rec_flag = 0;
			counter = 0;
            state = 1;

			m_rf_read(buffer, PACKET_LENGTH);
			int raw_f = *(int*)&buffer[0];
            duration = (int) buffer[2];

            double f = raw_f / 10.0;
            int a_time = 62550/f;

            OCR1A = a_time;
            OCR1B = a_time/2;

            set(TCCR1B, CS12);        // Turn on timer
            set(PORTB, 1);
		}

		if (check(TIFR3, OCF3A)) {
			set(TIFR3, OCF3A);
			counter++;
		}

		if (state && (counter >= duration)) {
			state = 0;
            clear(TCCR1B, CS12);        // Turn off timer
            clear(PORTB, 1);
		}
	}

	return 1;
}

void init() {
	m_red(ON);
	sei();
	m_clockdivide(0);
	m_bus_init();
	set(EIMSK, 2);
	while (!m_rf_open(CHANNEL, ADDRESS, PACKET_LENGTH));

    set(DDRB, 1);

	// PWM Setup
	pwm_setup();

	// Timer 3
    set(TCCR3B, CS32);
    clear(TCCR3B, CS31);
    clear(TCCR3B, CS30);

    clear(TCCR3B, WGM33);
    set(TCCR3B, WGM32);
    clear(TCCR3A, WGM31);
    clear(TCCR3A, WGM30);

    set(DDRC, 6);
    OCR3A = 625;
    m_red(OFF);
}

void pwm_setup() {

    // Clock source config
    clear(TCCR1B, CS12);        // Turn off timer
    clear(TCCR1B, CS11);        // ^
    clear(TCCR1B, CS10);        // ^

    // Timer mode config
    set(TCCR1B, WGM13);         // Use mode 15
    set(TCCR1B, WGM12);         // ^
    set(TCCR1A, WGM11);         // ^
    set(TCCR1A, WGM10);         // ^

    // Channel A config
    set(DDRB, 5);               // Set B5 to output

    // Channel B config
    set(DDRB, 6);               // Set B6 to output
    set(TCCR1A, COM1B1);        // Clear at B, set at A
    clear(TCCR1A, COM1B0);      // ^

    // Channel Compare config
    OCR1A = 62500/DEF_F;              // 1Hz frequency
    OCR1B = 62500/DEF_F/2;            // 50% duty cycle
}

ISR(INT2_vect) {
	rec_flag = 1;
}