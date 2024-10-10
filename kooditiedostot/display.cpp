#include "display.h"

// Pinneille kytkennät siirtorekisteriin
int latchPin = 5;  // Kytketty ST_CP:hen 74HC595:ssä
int clockPin = 6;  // Kytketty SH_CP:hen 74HC595:ssä
int dataPin = 7;   // Kytketty DS:ään 74HC595:ssä

// Taulukko numerolle 0-9 seitsemän segmentin näytölle
byte sevenSegDigits[] = {
  B00111111, // 0
  B00000110, // 1
  B01011011, // 2
  B01001111, // 3
  B01100110, // 4
  B01101101, // 5
  B01111101, // 6
  B00000111, // 7
  B01111111, // 8
  B01101111  // 9
};

int loopCounter = 0; //ei tarvi lopulliseen toteutukseen

void initializeDisplay(void) {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  /*
  pinMode(resetPin, OUTPUT); 
  pinMode(shiftClockPin, OUTPUT); 
  pinMode(latchClockPin, OUTPUT);
  pinMode(outEnablePin, OUTPUT); 
  pinMode(serialInputPin, OUTPUT);     //pinnit määritetään lähdöiksi 

  digitalWrite(resetPin, HIGH);          //nollaa siirtorekisterin
  digitalWrite(latchClockPin, LOW);     //kelloimpulssi  
  digitalWrite(shiftClockPin, LOW);     //näytön päivitys, lukitsee siirretyt bitit rekisteristä näytölle?
  digitalWrite(outEnablePin, LOW);      //näyttö päivittyy
  digitalWrite(serialInputPin, LOW);    //lähettää yksittäiset bitit rekisteriin sarjamuodossa
  */

  Serial.println("Display alustettu");
}

/*
void writeByte(uint8_t number, bool last) {
  for (int i = 7; i >= 0; i--) { 
    digitalWrite(serialInputPin, (number >> i) & 1);
    digitalWrite(shiftClockPin, HIGH);
    delay(50); ///????
    digitalWrite(shiftClockPin, LOW);  // 
  }

  if (last) {
    digitalWrite(latchClockPin, HIGH);   
    delay(50); //????
    digitalWrite(latchClockPin, LOW);
  }
}
*/

void writeHighAndLowNumber(uint8_t tensDigit, uint8_t onesDigit, bool rightDot, bool leftDot) {
 
  digitalWrite(latchPin,LOW); //valmistellaan datan lähetys
  // jos rightdot on true, lisätään desimaalipiste oikean näytön numeroon
  shiftOut(dataPin, clockPin, MSBFIRST, rightDot ? (sevenSegDigits[onesDigit] | B10000000) : sevenSegDigits[onesDigit]);
  //jos leftdot on true, lisätään desimaalipiste vasemman näytön numeroon
  shiftOut(dataPin, clockPin, MSBFIRST, leftDot ? (sevenSegDigits[tensDigit] | B10000000) : sevenSegDigits[tensDigit]);
  digitalWrite(latchPin,HIGH); //lukitaan data
  /*
  writeByte(segmentCodes[tens], false);
  writeByte(segmentCodes[ones], true);
  */
}

// näytetään "GO"
void showGO() {
writeHighAndLowNumber(6, 0, false, false);
}

void showResult(byte number) {

  uint8_t tens = number / 10; // laskee kymmenluvut
  uint8_t ones = number % 10; //laskee ykkösluvut
  writeHighAndLowNumber(tens, ones, false, false); //kirjoittaa luvut näytölle
} 
