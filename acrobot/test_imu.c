#include "m_general.h"
#include "m_bus.h"
#include "m_usb.h"
#include "m_imu.h"
#include <stdlib.h>
#include <math.h>


int main() {

	m_clockdivide(0);
	m_red(ON);
	m_usb_init();
	while(!m_imu_init(2,3));
	m_red(OFF);

	int buffer[9];

	while (1) {
		m_imu_raw(buffer);
		int i;
		for (i=0;i<8;i++) {
			m_usb_tx_int(buffer[i]);
			m_usb_tx_string("\t");
		}

		m_usb_tx_int(buffer[8]);
		m_usb_tx_string("\r\n");

	}

	return 1;
}