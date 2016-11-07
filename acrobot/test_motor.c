#include "m_general.h"

#define DEF_F 20000
#define DTC 0.5

void pwm_setup();

int main() {


	m_clockdivide(0);

	// Timer 1
	pwm_setup();

	set(DDRC, 6);
	set(DDRC, 7);


	// IN 1
	set(PORTC, 6);

	// IN 2
	clear(PORTC, 7);


	while (1) {


	}

	return 1;
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
    OCR1A = 62500/DEF_F;              // 10Hz frequency
    OCR1B = DTC*62500/DEF_F;            // 50% duty cycle

    set(TCCR1B, CS12);			// 256
}