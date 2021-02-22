/*
   sea-222-2021.pde
   T Czerwonka WO9U
   February 2021
   https://github.com/tczerwonka/sea-222

   Project to take the ESP504 radio move it to the US 222 amateur
   band.  The 2022 code (see elsewhere in the project) was minimally
   working but really needs to be completely re-written.
   Additionally decent functions for the PLL needed to be written
   then and that's still true today.
   Plus the A register for the PLL is 63/64 and that's it.

*/

#include "pindefs.h"

char szStr[20];

//initial values for the PLL -- all values in Hz
unsigned long l_frequency = 222100000; 
unsigned long l_step = 5000;
unsigned long l_reference_oscillator = 10275000;
int modulus = 64; //fixed on this PLL

void setup() {
  Serial.begin(9600);
  delay(5000);
  Serial.println("========================================");
  pinMode(A_0, OUTPUT);
  pinMode(A_1, OUTPUT);
  pinMode(A_2, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(SpiEn, OUTPUT);
  pinMode(LockDet, INPUT);
  pinMode(AdcCsOut, OUTPUT);
  pinMode(PttIn, INPUT);
  pinMode(SCKLOCAL, OUTPUT);
  pinMode(MOSILOCAL, OUTPUT);
  pinMode(MISOLOCAL, INPUT);
  pinMode(U3SW, OUTPUT);
  pinMode(BEEP, OUTPUT);
  pinMode(CW, INPUT);
  pinMode(CCW, INPUT);


  load_frequency(l_frequency);
  
  //turn on radio -- U3SW is ON if the radio is ON.
  radio_enable(1);

  //set frequency to 222.100
  load_frequency(l_frequency);
  //load_freq();
  //delay(100);
  //set the DAC values

  //values appropriate for SN WA2285
  //setDAC(60, 35, 38, 43);

  //delay(100);
  //wow this works!
  //beep(1000, 150);
  //beep(1500, 100);


}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  l_frequency += 5000;
  //load_frequency(l_frequency);

  Serial.println("X");

}



////////////////////////////////////////////////////////////////////////////////
// radio_enable()
//  turn on the radio
////////////////////////////////////////////////////////////////////////////////
void radio_enable(int power_state) {
  if (power_state == 1) {
    digitalWrite(U3SW, 1);
    Serial.println("radio power on");
  } else {
    digitalWrite(U3SW, 0);
    Serial.println("radio power off");
  }
} //radio_enable



////////////////////////////////////////////////////////////////////////////////
// load_frequency()
//    load a frequency into the MC145158 PLL
//    Frequency input is in hz
////////////////////////////////////////////////////////////////////////////////
void load_frequency(unsigned long frequency) {

  unsigned long NA;
  
  Serial.print("desired frequency: ");
  sprintf( szStr, "%09lu", l_frequency );
  Serial.println( szStr );

  //calculate target frequency
  unsigned long l_target = l_frequency + l_reference_oscillator;
  Serial.print("target frequency: ");
  sprintf( szStr, "%09lu", l_target );
  Serial.println( szStr );

  //R register is reference oscillator divided by desired step
  int R = l_reference_oscillator / l_step;
  Serial.print("R: ");
  Serial.println(R);

  //need to find the N and A
  //l_target / step = some large integer
  //large integer divided by modulus is some whole and some remainder
  //whole is N, remainder is A
  unsigned long l_divisor = l_frequency / l_step;
  unsigned long N = l_divisor / modulus;
  Serial.print("N: ");
  Serial.println(N);

  unsigned long A = l_divisor - (N * modulus);
  Serial.print("A: ");
  Serial.println(A);

  //R range -- 3 to 16383
  //N range -- 3 to 1023
  //if outside that set an error 
  //  (likely the step is too low)
  if (R > 16383) {
    Serial.println("ERROR: R exceeds 16383 - step too low");
    delay(1000); //raise some sort of general error in future
  }
  if (N > 1023) {
    Serial.println("ERROR: N exceeds 1023");
    delay(1000); //raise some sort of general error in future
  }

  //e.g.
  //desired frequency: 222100000
  //target frequency: 232375000
  //R: 2055
  //N: 694
  //A: 4

  //At this point we have R, N, A -- need to load into the PLL
  //R is 14 bits -- put one for control at LSB -- total of 15 bits 
  //A and N are 17 bits together, 7 bits for A, 10 bits for N
  //  -- then put one for control at LSB position -- 18 bits total
  //now shift out in 3 16-bit ints -- shift must go MSB first
  //shiftOut works on bytes -- something like this needs to be done
  //shiftOut(dataPin, clock, MSBFIRST, (data >> 8));

  Serial.print("R: 0x");
  sprintf( szStr, "%X", R );
  Serial.println( szStr );
  R = R << 1; //shift left
  Serial.print("R bsl: 0x");
  sprintf( szStr, "%X", R );
  Serial.println( szStr );
  //2055d -> 0x807 -> 0x100E
  //0b100000000111 -> 0b1000000001110
  R = R | 1; // OR the one at the LSB?
  Serial.print("R OR 1: 0x");
  sprintf( szStr, "%X", R );
  Serial.println( szStr );
  //0x100E -> 0x100f
  //0b1000000001110 -> 0b1000000001111 ---- looks right

  //N needs to shift up 8
  Serial.print("N: 0x");
  sprintf( szStr, "%X", N );
  Serial.println( szStr );
  N = N << 8; //shift left
  Serial.print("N bsl8: 0x");
  sprintf( szStr, "%X", N );
  Serial.println( szStr );
  //N: 0x2B6
  //N bsl8: 0xB600
  //0b1010110110 -> 0b1011011000000000

  //WRONG WRONG -- NA -- total of 17 bits plus another shift
  Serial.print("A: 0x");
  sprintf( szStr, "%X", A );
  Serial.println( szStr );
  NA = N | A; // OR the two together
  Serial.print("NA: 0x");
  sprintf( szStr, "%X", NA );
  Serial.println( szStr );
  NA = NA << 1; // should OR in a 0 at LSB but that's assumed here
  Serial.print("NA bsl: 0x");
  sprintf( szStr, "%X", NA );
  Serial.println( szStr );




  



  //shiftOut(dataPin, clock, MSBFIRST, R);
  //shiftOut(dataPin, clock, MSBFIRST, (R >> 8));
  //shiftOut(dataPin, clock, MSBFIRST, NA);
  //shiftOut(dataPin, clock, MSBFIRST, (NA >> 8));



  
  
} //load_frequency
