/*

DDS Waveform generator for Arduino Due. Uses interupts to cycle through Wavetable.


*/

#define maxSamplesNum 50000
#include "Waveforms.h"


int iSample = 0;

uint32_t dacout = waveformsTable[iSample];

void setup() {

	pinMode(41, OUTPUT);
	digitalWriteDirect(41, 0);

	delay(1);

	pmc_set_writeprotect(false);		 // disable write protection for pmc registers
	pmc_enable_periph_clk(ID_TC3);	 // enable peripheral clock TC4 this means T4 on TC1 channel 1  - this is the timer for the stim trigger output
	// set up timers and interupts - set channel on timers, set to "wave mode" meaning an output rather than "capture" to read ticks
	TC_Configure(TC1, 0, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1); // use TC1 channel 1 in "count up mode" using MCLK /2 clock1 to give 42MHz

	TC_SetRC(TC1, 0,84); // count 63 ticks on the 42MHz clock before calling the overflow routine - this gives an interupt every 1.5 uS
						  // enable timer interrupts on the timer for stim tigger output
	TC1->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;   // IER = interrupt enable register
	TC1->TC_CHANNEL[0].TC_IDR = ~TC_IER_CPCS;  // IDR = interrupt disable register
											   // enable timer interrupts on the timer 

											   //Enable the interrupt in the nested vector interrupt controller
  //TC4_IRQn where 4 is the timer number * timer channels (3) + the channel number (=(1*3)+1) for timer1 channel1
	NVIC_EnableIRQ(TC3_IRQn);

	//Write the DACs
	//dacc_set_channel_selection(DACC_INTERFACE, 0);       //select DAC channel 0
	//dacc_write_conversion_data(DACC_INTERFACE, out_DAC0);//write on DAC
	//dacc_set_channel_selection(DACC_INTERFACE, 1);       //select DAC channel 1
	//dacc_write_conversion_data(DACC_INTERFACE, out_DAC1);//write on DAC

	TC_Start(TC1, 0);



	Serial.begin(115200);
	Serial.println("hello, making sine wave pulses");
	//this is a cheat to enable DAC
	analogWrite(DAC1, 0);

}

void loop() {

}

// MAKE SMALLER INTS SO WE CAN STORE MORE!


void TC3_Handler() //this is the ISR for the 667kHz timer - runs every 1.5 uS - this is as fast as I could reliably get it as worst case code takes 1.357 uS to run
{
	// We need to get the status to clear it and allow the interrupt to fire again
	TC_GetStatus(TC1, 0); //here TC2,1 means TIMER 2 channel 1

	digitalWriteDirect(41, 1);

	//Serial.print("1");

	dacout = waveformsTable[iSample];
	//dacc_set_channel_selection(DACC_INTERFACE, 1);       //select DAC channel 1
	dacc_write_conversion_data(DACC_INTERFACE, dacout);//write on DAC
	//analogWrite(DAC1, dacout);

	iSample++;

	if (iSample == maxSamplesNum)
	{
		iSample = 0;
	}


	digitalWriteDirect(41, 0);

}

//taken from http://forum.arduino.cc/index.php?topic=129868.15
inline void digitalWriteDirect(int pin, int val) {
	if (val) g_APinDescription[pin].pPort->PIO_SODR = g_APinDescription[pin].ulPin;
	else    g_APinDescription[pin].pPort->PIO_CODR = g_APinDescription[pin].ulPin;
}

