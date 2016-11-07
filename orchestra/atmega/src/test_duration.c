// Kevin Lu

#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include <math.h>

#define PRESCALE 0
#define CHANNEL 1
#define ID 0x40
#define PACKET_LENGTH 3

volatile int rec_flag = 0;
volatile int inc_flag = 0;

void init();

int main() {

    int dc = 0;
    int dur = 500;
    int state = 0;
    int buffer[3] = {0, 0, 0};

    init();
    while (1) {

        if (check(TIFR3, OCF3A)) {
            set(TIFR3, OCF3A);
            dc++;
        }

        if (rec_flag) {
            rec_flag = 0;
            state = 1;
            dc = 0;
            m_rf_read(buffer, PACKET_LENGTH);
            m_red(TOGGLE);
        }

        if (state && (dc >= dur)) {
            m_red(OFF);
            state = 0;
        }
    }

    return 0;
}

void init() {
    m_red(ON);
    sei();
    m_clockdivide(PRESCALE);

    // RF
    m_bus_init();
    set(EIMSK, 2);
    m_rf_open(CHANNEL, ID, PACKET_LENGTH);

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

ISR(INT2_vect) {
    rec_flag = 1;
}
