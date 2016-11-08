#include "m_general.h"
#include "m_bus.h"
#include "m_usb.h"
#include "m_imu.h"
#include <stdlib.h>
#include <math.h>

#define CLOCK_PRESCALE 0
#define ACCEL_PRESCALE 0
#define GYRO_PRESCALE 3
#define ACCEL_RANGE 2
#define GYRO_RANGE 2000
#define DELTA_T 0.00005
#define GYRO_SCALE 53.0
#define WHEEL_RADIUS 1		// INCH

void init();
void timer_setup();
void m_usb_tx_data(int *data, int size);
void pwm_setup();

volatile int imu_update_flag = 0;
volatile double goffset = 40.0;

int main() {

	// Iniatilization Steps
	init();
	int data[9];				// Buffer for holding IMU Data
	double d_angle = 0;			// Angular Velocity
	double angle = 0;			// Current tilt angle
	
	// Feedback Constants
	double Kp = 3;//10;			// Proportional control constant
	double Ki = 0;			// Integral control constant
	double Kd = 6;//60;			// Derivative control constant

	// Feedback variable
	double error = 0;
	double old_error = 0;
	
	while (1) {

		if (imu_update_flag) {
			imu_update_flag = 0;

			// If IMU read is successful
			if (m_imu_raw(data)) {
				m_usb_tx_data(data, 9);
				d_angle = (data[3]+goffset) / 32768.0;
				d_angle *= GYRO_RANGE;
				d_angle *= DELTA_T;
				d_angle *= GYRO_SCALE;

				double ay = data[1]-1500.0;
				double az = data[2]+200.0;

				double den = 57.29*atan2(ay,az) + 3.5;
				angle = 0.98*(angle+d_angle) + 0.02*den;

				// Controller
				error = -angle;
				double d_error = (error - old_error);
				double i_error = (error + old_error)*0.5;
				double control = Kp*error + Kd*d_error + Ki*i_error;
				double mag = abs(control);
				if (mag > 25) {
					mag = 25;
				}
				

				// Motor control
				int b_time = (int) (150.0*(mag/30.0));
				set(PORTB, 1);				// Standby = high
				if (abs(control) < 2) {	// Set margins to the value to check

					clear(PORTC, 6);
					clear(PORTC, 7);

				} else {

					OCR1B = b_time;

					if (control < 0) {

						set(PORTC, 6);			// Drive in one direction
						clear(PORTC, 7);

					} else if (control > 0) {

						clear(PORTC, 6);		// Drive in the other
						set(PORTC, 7);

					}
				}

				// Update Values
				old_error = error;

				m_usb_tx_string("\r\n");

			}
		}
	}

	return 1;
}

void init() {
	// 
	m_red(ON);
	m_clockdivide(CLOCK_PRESCALE);
	sei();
	m_bus_init();
	m_usb_init();
	// while (!m_usb_isconnected());
	while (!m_imu_init(ACCEL_PRESCALE, GYRO_PRESCALE));
	timer_setup();
	pwm_setup();



	// Motors
	set(DDRB, 1);	// STANDBY
	set(DDRC, 6);	// AIN1, BIN1
	set(DDRC, 7);	// AIN2, BIN2
	m_red(OFF);
}

void m_usb_tx_data(int *data, int size) {
	int i;
	for (i=0; i<size; i++) {
		m_usb_tx_int(data[i]);
		m_usb_tx_char('\t');
	}
}

void timer_setup() {
	// User Timer 0 at /8 prescale
	clear(TCCR0B, CS02);
	set(TCCR0B, CS01);
	set(TCCR0B, CS00);	

	// Count up to OCR0A
	clear(TCCR0B, WGM02);
	set(TCCR0A, WGM01);
	set(TCCR0A, WGM00);

	OCR0A = 100;			// Update at 20kHz

	// Enable OVF interrupt
	set(TIMSK0, TOIE0);
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
    OCR1A = 160;              // 100kHz frequency
    OCR1B = 120;           	  // 50% duty cycle

    set(TCCR1B, CS10);			// /1
}

// Timer 0 - imu update timer 
ISR(TIMER0_OVF_vect) {
	imu_update_flag = 1;
}