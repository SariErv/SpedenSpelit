#include "display.h"

int resetPin = 7;
int shiftClockPin = 6;
int latchClockPin = 5;
int outEnablePin = 4;
int serialInputPin = 3;   //pinnien määrittely arduinolle

uint8_t segmentCodes [] = {

    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9  // segmenttikoodit numeroille 0-9
  };


void initializeDisplay(void) {

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
}

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

void writeHighAndLowNumber(uint8_t tens, uint8_t ones) {
 
  writeByte(segmentCodes[tens], false);
  writeByte(segmentCodes[ones], true);
}

void showResult(byte number) {

  uint8_t tens = number / 10; // laskee kymmenluvut
  uint8_t ones = number % 10; //laskee ykkösluvut
  writeHighAndLowNumber(tens, ones); //kirjoittaa luvut näytölle
}

void setup() {
  initializeDisplay();   
  showResult(0);
}

void loop() {
