#include "buttons.h"

volatile int pressedButton = -1; // -1 toimii merkkausarvona, että mikään nappi ei ole aktiivinen aluksi

const int buttonPins[] = {10,11,12,13}; // Käytetään taulukkoa, jotta voidaan käsitellä yksinkertaisemmin useampia pinnejä

// Debounce-muuttuja
unsigned long interruptTime = 0; // Tallentaa ajan viimeisestä keskeytyksestä


void initButtonsAndButtonInterrupts(void)
{

  PCICR |= (1 << PCIE0); // Tällä mahdollistetaan PCINT0 -väylän keskeytykset

  // Sallitaan erikseen vielä käytetyille pinneille keskeytykset

  PCMSK0 |= (1 << PCINT2); // 2 viittaa pinniin 10
  PCMSK0 |= (1 << PCINT3); // 3 viittaa pinniin 11
  PCMSK0 |= (1 << PCINT4); // 4 viittaa pinniin 12
  PCMSK0 |= (1 << PCINT5); // 5 viittaa pinniin 13

  for(int i = 0;i<4;i++)
  {
    pinMode(buttonPins[i], INPUT_PULLUP);  // Asetetaan pinnit 10-13 INPUT_PULLUP tilaan, jotta kytkimiin ei tule häiriöitä
  }
  Serial.print("Nappien pinnit alustettu");
  // See requirements for this function from buttons.h

}

ISR(PCINT0_vect) {

unsigned long debounceTimer = millis(); // Otetaan nykyinen aika millisekunteina

  if (debounceTimer - interruptTime > 200)
  {
    for(int i = 0;i<4;i++)
    {
      byte pressed = digitalRead(buttonPins[i]); // Käytetään digitalReadille sopivaa tietotyyppiä byte. Tämä funktio lukee pinnin i arvon ja tallentaa sen pressed -muuttujaan.

      if(pressed == LOW) // Jos joku painike painettu...
      {
        pressedButton = i; // ... painikkeen pinnin arvo tallennetaan muuttujaan pressedButton
        interruptTime = debounceTimer; // Tallennetaan muuttujaan edellisen keskeytyksen aika
        break; // Päätetään tämä silmukka, kun painettu nappi löydetty
      }
    }
  }

   /*
     Here you implement logic for handling
	 interrupts from 2,3,4,5 pins for Game push buttons
	 and for pin 6 for start Game push button.
   */

}
