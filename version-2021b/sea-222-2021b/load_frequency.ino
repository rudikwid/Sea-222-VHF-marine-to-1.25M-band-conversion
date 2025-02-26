////////////////////////////////////////////////////////////////////////////////
// load_frequency()
//    load a frequency into the MC145158 PLL
//    Frequency input is in hz
//
//finished load_frequency

//
////////////////////////////////////////////////////////////////////////////////
void load_frequency(unsigned long frequency) {
  Serial.print("load_frequency: ");
  Serial.println(frequency);

  //Serial.print("desired frequency: ");
  //sprintf( szStr, "%09lu", l_frequency );
  //Serial.println( szStr );

  //calculate target frequency
  unsigned long l_target = l_frequency + l_reference_oscillator;
  //Serial.print("target frequency: ");
  //sprintf( szStr, "%09lu", l_target );
  //Serial.println( szStr );

  //R register is reference oscillator divided by desired step
  int R = l_reference_oscillator / l_step;
  //Serial.print("R: ");
  //Serial.println(R);

  //need to find the N and A
  //l_target / step = some large integer
  //large integer divided by modulus is some whole and some remainder
  //whole is N, remainder is A
  unsigned long l_divisor = l_frequency / l_step;
  unsigned long N = l_divisor / modulus;
  //Serial.print("N: ");
  //Serial.println(N);

  unsigned long A = l_divisor - (N * modulus);
  //Serial.print("A: ");
  //Serial.println(A);

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

  //this could all be condensed in a few statements and potentially
  //simplified but when I look at this (again) in ten years it's less
  //likely that I'll curse the long description than a summary
  //my view is the MSB is on the "left"

  //At this point we have R, N, A -- need to load into the PLL
  //R is 14 bits -- put one for control at LSB -- total of 15 bits 
  //A and N are 17 bits together, 7 bits for A, 10 bits for N
  //  -- then put one for control at LSB position -- 18 bits total
  //now shift out in 3 16-bit ints -- shift must go MSB first
  //shiftOut works on bytes -- something like this needs to be done
  //shiftOut(dataPin, clock, MSBFIRST, (data >> 8));
  
  //ideal
  //RRRRRRRRRRRRRRC NNNNNNNNNNAAAAAAAC
  //M            L1 M        LM     L0
  // S            S  S        SS     S
  //  B            B  B        BB     B
 
  //OH I SUPPOSE
  //RRRRRRRRRRRRRR1 EN 000000NN NNNNNNNNAAAAAAA0 EN
  //-------R-------    ---MM--- -------NN-------

  //Serial.print("R: 0x");
  //sprintf( szStr, "%X", R );
  //Serial.println( szStr );
  R = R << 1; //shift left
  //Serial.print("R bsl: 0x");
  //sprintf( szStr, "%X", R );
  //Serial.println( szStr );
  //2055d -> 0x807 -> 0x100E
  //0b100000000111 -> 0b1000000001110
  R = R | 1; // OR the one at the LSB?
  //Serial.print("R OR 1: 0x");
  //sprintf( szStr, "%X", R );
  //Serial.println( szStr );
  //0x100E -> 0x100f
  //0b1000000001110 -> 0b1000000001111 ---- looks right


  //need two MSB bits of a 10-bit N in LSB-most position of a unsigned int
  // e.g. 000000NNNNNNNNNN -> 000000NN + NNNNNNNN00000000
  // NNNNNNNN--------
  
  //   -- make a copy of N, shift down 8 bits as N is only 10 bits
  unsigned long N_PART;
  N_PART = N >> 8;
  //now shift N up 8 bits -- that will roll off the first two and leave the last 8 as zero
  N = N << 8;
  //shift A up one bit - LSB becomes control zero
  A = A << 1;
  //OR A on lower part of N
  N = N | A;

  //Serial.print("N_PART: 0x");
  //sprintf( szStr, "%X", N_PART );
  //Serial.println( szStr );

  //Serial.print("N: 0x");
  //sprintf( szStr, "%X", N );
  //Serial.println( szStr );

  //desired frequency: 222100000
  //target frequency: 232375000
  //R: 2055
  //N: 694 -- 0x2B6 -- 0b00000010 10110110
  //A: 4
  //R: 0x807
  //R bsl: 0x100E
  //R OR 1: 0x100F
  //N_PART: 0x2 -- right -- that's just 0b10
  //N: 0xB608 -- 0b10110110 0000100 0

  digitalWrite(SpiEn, 1);
  shiftOut(MOSILOCAL, SCKLOCAL, MSBFIRST, R);
  shiftOut(MOSILOCAL, SCKLOCAL, MSBFIRST, (R >> 8));
  //now toggle ENABLE
  U4_control(PLL_PGM);
  U4_control(RESET);

  shiftOut(MOSILOCAL, SCKLOCAL, MSBFIRST, N_PART);
  shiftOut(MOSILOCAL, SCKLOCAL, MSBFIRST, N);
  shiftOut(MOSILOCAL, SCKLOCAL, MSBFIRST, (N >> 8));
  //now toggle ENABLE
  U4_control(PLL_PGM);
  U4_control(RESET);
  digitalWrite(SpiEn, 0);
  
  Serial.println("finished load_frequency");
} //load_frequency
