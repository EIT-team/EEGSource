/*

DDS Waveform generator for Arduino Due. Uses interupts to cycle through Wavetable.

Wave every second through another timer

*/

#define maxSamplesNum 25000
#include "WaveformsEEG.h"


int StartSample = 0;
int EndSample = maxSamplesNum;

int Start_pad = 12500; //maxSamplesNum - 25000; //12500
int End_pad = 12500; //12500




int iSample = StartSample;
int iRep = 0;

int maxReps = 3;

uint32_t dacout = waveformsTable[iSample];

void setup() {

	pinMode(41, OUTPUT);
	digitalWriteDirect(41, 0);

	//	delay(1);

	pmc_set_writeprotect(false);		 // disable write protection for pmc registers
	pmc_enable_periph_clk(ID_TC3);	 // enable peripheral clock TC3 this means T3 on TC1 channel 0  - this is the timer for the stim trigger output
	pmc_enable_periph_clk(ID_TC1);	 // enable peripheral clock TC1 this means T3 on TC0 channel 1  - this is the timer for the stim trigger output

	 // set up timers and interupts - set channel on timers, set to "wave mode" meaning an output rather than "capture" to read ticks
	TC_Configure(TC1, 0, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1); // use TC1 channel 0 in "count up mode" using MCLK /2 clock1 to give 42MHz
	TC_Configure(TC0, 1, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4); // use TC0 channel 1 in "count up mode" using MCLK /128 clock1 to give 65.625 kHz


	TC_SetRC(TC1, 0, 840); // count 840 ticks on the 42MHz clock before calling the overflow routine - this gives an interupt every 20 us
	//TC_SetRC(TC0, 1, 656250); // count 656250 ticks on the 65.625 kHz clock before calling the overflow routine - this gives an interupt every 1 s
	TC_SetRC(TC0, 1, 328125); // count 328125 ticks on the 65.625 kHz clock before calling the overflow routine - this gives an interupt every 0.5 s

	// enable timer interrupts on the timer for DAC and Pulse timers
	TC1->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;   // IER = interrupt enable register
	TC1->TC_CHANNEL[0].TC_IDR = ~TC_IER_CPCS;  // IDR = interrupt disable register
	TC0->TC_CHANNEL[1].TC_IER = TC_IER_CPCS;   // IER = interrupt enable register
	TC0->TC_CHANNEL[1].TC_IDR = ~TC_IER_CPCS;  // IDR = interrupt disable register
											   // enable timer interrupts on the timer 
	//Enable the interrupt in the nested vector interrupt controller
	//TC3_IRQn where 3 is the timer number * timer channels (3) + the channel number (=(1*3)+0) for timer1 channel0
	NVIC_EnableIRQ(TC3_IRQn);
	//TC1_IRQn where 1 is the timer number * timer channels (3) + the channel number (=(0*3)+1) for timer0 channel1
	NVIC_EnableIRQ(TC1_IRQn);
	//Write the DACs
	//dacc_set_channel_selection(DACC_INTERFACE, 0);       //select DAC channel 0
	//dacc_write_conversion_data(DACC_INTERFACE, out_DAC0);//write on DAC
	//dacc_set_channel_selection(DACC_INTERFACE, 1);       //select DAC channel 1
	//dacc_write_conversion_data(DACC_INTERFACE, out_DAC1);//write on DAC

	//TC_Start(TC1, 0);
	TC_Start(TC0, 1);

	Serial.begin(115200);
	Serial.println("hello, making sine wave pulses");
	//this is a cheat to enable DAC
	analogWrite(DAC1, 0);

}

void loop() {

}


void TC3_Handler() //this is the ISR for the 500kHz timer - runs every 2 uS 
{
	// We need to get the status to clear it and allow the interrupt to fire again
	TC_GetStatus(TC1, 0); //here TC2,1 means TIMER 2 channel 1

	//digitalWriteDirect(41, 1);

	dacc_write_conversion_data(DACC_INTERFACE, waveformsTable[iSample]);//write on DAC DACC_INTERFACE is currently *already* set to DAC1 from the original AnalogWrite call

	iSample++;

	// if we have finished wavetable
	if (iSample == EndSample)
	{
		iSample = StartSample; //reset counter
		iRep++;

		if (iRep == maxReps - 1)
		{
			EndSample = End_pad; // if we are on last loop then only go to the pad end
		}
	}
	// if we are on last loop
	if (iRep == maxReps)
	{

		TC_Stop(TC1, 0); //stop this ISR
	}


	//digitalWriteDirect(41, 0);

}

void TC1_Handler() //this is the ISR for the 2Hz timer
{
	// We need to get the status to clear it and allow the interrupt to fire again
	TC_GetStatus(TC0, 1); //here TC2,1 means TIMER 2 channel 1

	digitalWriteDirect(41, 1);

	iRep = 0;
	iSample = Start_pad;
	EndSample = maxSamplesNum;

	TC_Start(TC1, 0); //start the DAC timer ISR

	digitalWriteDirect(41, 0);

}



//taken from http://forum.arduino.cc/index.php?topic=129868.15
inline void digitalWriteDirect(int pin, int val) {
	if (val) g_APinDescription[pin].pPort->PIO_SODR = g_APinDescription[pin].ulPin;
	else    g_APinDescription[pin].pPort->PIO_CODR = g_APinDescription[pin].ulPin;
}

