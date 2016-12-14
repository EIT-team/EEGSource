/*
  Simple Waveform generator with Arduino Due

    connect two push buttons to the digital pins 2 and 3
    with a 10 kilohm pulldown resistor to choose the waveform
    to send to the DAC0 and DAC1 channels
    connect a 10 kilohm potentiometer to A0 to control the
    signal frequency

*/

#define maxSamplesNum 1000
#include "Waveforms.h"


int iSample = 0;

unsigned long sampletimeold; //tracking time for loop iteration
unsigned long sampletimenew;


unsigned int delaytime = 0; //microseconds to delay iteration
int tdiff = 0; //difference in

void setup() {
  analogWriteResolution(12);  // set the analog output resolution to 12 bit (4096 levels)
  analogReadResolution(12);   // set the analog input resolution to 12 bit

  Serial.begin(115200);

  Serial.println("hello, making sine wave pulses");



}

void loop() {

  makesine();
  iSample = 0;
  delay(1000);





}


void makesine()
{
  

  for(iSample =0;iSample <maxSamplesNum; iSample++)
  {
  
  sampletimeold = micros();
  analogWrite(DAC1, waveformsTable[iSample]);  // write the selected waveform on DAC0
//  iSample++;
//  if (iSample == maxSamplesNum) // Reset the counter to repeat the wave
//  {
//    iSample = 0;
//  }



  sampletimenew = micros();
  tdiff = sampletimenew - sampletimeold;
  //    Serial.print("tdiff is:");
  //    Serial.println(tdiff);
  delaytime = 100 - tdiff;
  //  Serial.print("delaytime is:");
  //  Serial.println(delaytime);
  delayMicroseconds(delaytime);
  //   sampletimenew=micros();
  //   Serial.println(sampletimenew-sampletimeold);

}

}




