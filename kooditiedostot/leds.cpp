#include "leds.h"

const int ledPins[] = {15,16,17,18}; // ledPins[0] => 15 (A1), ledPins[1] => 16 (A2), ledPins[2] => 17 (A3), ledPins[3] => 18 (A4)

int activedLed = -1; // -1 toimii merkkausarvona, että mikään led ei ole aktiivinen aluksi

void initializeLeds()
{
  for(int i = 0;i<4;i++)
  {
    pinMode(ledPins[i], OUTPUT); // Alustetaan pinnit outputeiksi
  }
  clearAllLeds();
  Serial.println("LEDit alustettu");
}


void setLed(byte ledNumber)
{
  clearAllLeds();
  if (ledNumber >= 0 && ledNumber < 4) { // Mahdollistetaan ledin sytyttäminen vain lediksi määritetyille pinneille
      digitalWrite(ledPins[ledNumber], HIGH); // Sytytetään haluttu led
      activedLed = ledNumber; // Tallennetaan activedLed muuttujaan, mikä ledi on sytytetty
  }
}


void clearAllLeds()
{
 for(int i = 0;i<4;i++)
  {
    digitalWrite(ledPins[i], LOW); // Sammuttaa kaikki ledit
  }
}


void setAllLeds()
{
 for(int i = 0;i<4;i++)
  {
    digitalWrite(ledPins[i], HIGH); // Sytyttää kaikki ledit
  }
}


void show1()
{
  for(int i = 0;i<=15;i++) // Käy läpi luvut 0-15
  {
    for(int j =0;j<4;j++) // Käy läpi jokaisen ledin
    {
      if(i &(1<<j)) // Kasvatetaan binäärilukua
      {
        digitalWrite(ledPins[j], HIGH);
      }
      else
      {
        digitalWrite(ledPins[j], LOW);
      }
      if(timeToCheckGameStatus) //jos nappia painetaan show1:n ollessa kaynnissa, sammutetaan kaikki ledit ja lopetetaan show1 keskenkaiken return-komennolla
      {
        timeToCheckGameStatus = false;
        clearAllLeds();
        return; 
      }
    }
    delay(500);
  }
  clearAllLeds();
}


void show2(int repeats)
{
  int lightshowDelay = 500; // Määritellään alkuviive
  for(int i = 0;i<repeats;i++) // Toistetaan kierros repeats muuttujaan määritellyn lukumäärän mukaan
  {
    for(int j = 0;j<4;j++) // Käy läpi jokaisen ledin
    {
      clearAllLeds();
      digitalWrite(ledPins[j], HIGH); // Sytytetään ledi
      delay(lightshowDelay); // Odotetaan hetki
      if(lightshowDelay > 50) // 50 pienemmällä viiveellä ei enää toisteta
      {
        lightshowDelay -= 10; // Vähennetään viivettä joka kierroksella
      }
      if(timeToCheckGameStatus) //jos nappia painetaan show2:n ollessa käynnissä, sammutetaan kaikki ledit ja lopetetaan show2 keskenkaiken return-komennolla
      {
        timeToCheckGameStatus = false;
        clearAllLeds();
        return;
      }
    }
  }
  clearAllLeds();
}

void flipOneLed(int ledNmbr)
{
  int state = digitalRead(ledPins[ledNmbr]);
  digitalWrite(ledPins[ledNmbr],!state);
}

void flipAllLeds()
{
  for(int i=0;i<4;i++)
  {
    flipOneLed(i);
  }
}
