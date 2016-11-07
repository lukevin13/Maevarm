// Kevin Lu

#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include <math.h>

#define PRESCALE 0
#define HZ_1500 10666

#define CHANNEL 1
#define ID 0x40
#define PACKET_LENGTH 3

volatile int rec_flag = 0;
volatile int duration_count = 0;         // Count duration

void init();
void rf_setup();
void pwm_setup();
void timer_setup();

// Main function
int main() {

    init();
    double t = 0;                      // Time elapsed in one second
    int state = 0;                  // 0 - off, 1 - on
    double f = 600;                   // Frequency
    int duration = 0;               // Duration
    
    char buffer[3] = {0, 0, 0};      // Buffer to hold packet values

    while (1) {
        if (check(TIFR1, TOV1)) {    // Flag sets 1500 times per second
            set(TIFR1, TOV1);        // Clear timer flag
            if (t == 1500) t = 0;    // Reset every second
            t++;                     // Increment time counter
        }

        if (rec_flag) {
            rec_flag = 0;
            state = 1;
            m_rf_read(buffer, PACKET_LENGTH);   // Read packets into the buffer
            duration = (int) buffer[2];               // Get duration
            duration_count = 0;                 // Reset timer count
            set(TCCR3B, CS32);                  // Turn on timer 3A
            m_red(TOGGLE);
        }

        if (state) {
            int raw_f = *(int*)&buffer[0];      // Desired output frequency (1/10 Hz)
            f = raw_f / 10.0;                   // Desired output frequency (Hz)
        } else {
            f = 0;
        }

        double voltage = 2.5 * sin(f*2*3.14*(t/1500.0)) + 2.5; // Output voltage at this time
        double duty = voltage/5;                // Get duty cycle
        int b_time = (int) (HZ_1500 * duty);    // Get timer value for duty cycle
        OCR1B = b_time;                         // Set B
    }

    return 0;
}

// Initialize
void init() {
    m_red(ON);

    sei();                      // Enable global interrupts
    m_clockdivide(PRESCALE);    // Apply system clock prescale
    rf_setup();                 // Set up RF module
    pwm_setup();                // Set up PWM
    // timer_setup();

    m_red(OFF);
}

// Setup RF Module
void rf_setup() {

    m_bus_init();               // Initialize the bus
    set(EIMSK, 2);              // Enable INT2
    m_rf_open(CHANNEL, ID, PACKET_LENGTH); // Open channel 

}

// Setup PWM
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
    OCR1A = HZ_1500;              // 1.5 kHz frequency
    OCR1B = HZ_1500/2;            // 50% duty cycle

    // Turn on timer A
    set(TCCR1B, CS10);          // Use /1 timer prescale
}

// // Setup Timer
// void timer_setup() {

//     // Clock source config
//     clear(TCCR3B, CS32);        // Turn off timer
//     clear(TCCR3B, CS31);        // ^
//     clear(TCCR3B, CS30);        // ^

//     // Timer mode config
//     clear(TCCR3B, WGM33);       // Use mode 4
//     set(TCCR3B, WGM32);         // ^
//     clear(TCCR3A, WGM31);       // ^
//     clear(TCCR3A, WGM30);       // ^

//     // Channel A config
//     set(DDRC, 6);               // Set C6 to output

//     // Channel Compare config
//     OCR1A = 625;                // count 1/100 sec

//     // Turn on timer A
//     set(TCCR3B, CS32);          // Use /256 timer prescale
// }

ISR(INT2_vect) {
    rec_flag = 1;               // Set the flag
}

// ISR(TIMER3_COMPA_vect) {
//     duration_count++;
// }