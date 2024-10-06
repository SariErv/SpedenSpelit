#include "display.h"
#include "buttons.h"
#include "leds.h"
#include "SpedenSpelit.h"

// Use these 2 volatile variables for communicating between
// loop() function and interrupt handlers
volatile int buttonNumber = -1;           // for buttons interrupt handler
volatile bool newTimerInterrupt = false;  // for timer interrupt handler

//muita pelissä tarvittavia muuttujia

volatile int timerSpeedCounter = 0;   // for keeping track of increasing game speed
volatile bool timeToCheckGameStatus = false;

byte randomNumbers[100];
volatile int userNumbers[100];

int timerInterruptionCounter = 0;
int currentScore = 0;

bool gameRunning = false;

int tempIndex = 0; //keksi parempi nimi muuttujalle (while(gameRunning == false)-haarassa)

int buttonInterruptionsCounter = 0;

void setup()
{
  /*
    Initialize here all modules
  */
  initButtonsAndButtonInterrupts();
  initializeDisplay();
  initializeLeds();
  initializeTimer();
  //Serial.println("odotetaan, että käyttäjä aloittaa pelin nappia painamalla ...");
}



void loop()
{
  while(gameRunning == false)
  {
    if(timeToCheckGameStatus == true)
    {
      userNumbers[tempIndex] = buttonNumber;
      tempIndex++;
      if((userNumbers[0] == 2 && userNumbers[1] == 3) || (userNumbers[0] == 3 && userNumbers[1] == 2))
      {
        startTheGame();
        //gameRunning = true; //sama koodirivi initializeGame()-funktiossa, jota startTheGame()-funktio kutsuu (ei siis tarvi tähän)
      }
      timeToCheckGameStatus = false;
      if(tempIndex > 1)
      {
        tempIndex = 0;
      }
    }
  }
  if(newTimerInterrupt == true)
  {
    // new random number must be generated
    // and corresponding let must be activated
    randomNumbers[timerInterruptionCounter] = random(0,4);  //arvo luku välillä 0-3
    setLed(randomNumbers[timerInterruptionCounter]);
    timerInterruptionCounter++;
    newTimerInterrupt = false; 
  }
  if(timeToCheckGameStatus == true)
  {
    userNumbers[buttonInterruptionsCounter] = buttonNumber;
    checkGame(buttonNumber);
    buttonInterruptionsCounter++;
    timeToCheckGameStatus = false;
  }
  if((timerInterruptionCounter - buttonInterruptionsCounter) > 10)
  {
    showResult(currentScore);
    stopTheGame();
  }
}

void initializeTimer()
{
	// see requirements for the function from SpedenSpelit.h
  cli(); //clear interrupts eli poistetaan keskeytykset käytöstä timerin alustuksen ajaksi
  //nollataan rekisterit (TCCR1A ja TCCR1B) sekä laskuri (TCNT1)
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  //asetetaan output compare rekisteri sellaiseen arvoon, jolla saadaan keskeytyksiä kerran sekunnissa
  OCR1A = 15624;        //(16 000 000)/(1 024*1) - 1 = 15624
  TCCR1B |= B00001101;  //WGM12=1, CS12=1, CS10=1 --> CTC-mode päälle, prescaleri=1024
  TIMSK1 |= B00000010;  //OCIE1A=1 --> output compare match A päälle
  //sei(); //set interrupts eli otetaan keskeytykset taas käyttöön -- HUOM! keskeytysten päällepano startTheGame() funktioon
}
ISR(TIMER1_COMPA_vect)
{
  /*
  Communicate to loop() that it's time to make new random number.
  Increase timer interrupt rate after 10 interrupts.
  */
  timerSpeedCounter++;
  newTimerInterrupt = true;
  if(timerSpeedCounter == 10) {
    OCR1A /= 1.1;   //lower the output compare register value, so that interrupts happen 10% more often
    timerSpeedCounter = 0;
  }
}

void checkGame(byte nbrOfButtonPush)
{
	// see requirements for the function from SpedenSpelit.h
  if(randomNumbers[currentScore] == nbrOfButtonPush)
  {
    showResult(currentScore + 1);
    currentScore++;
  }
  else 
  {
    stopTheGame();
  }
}

void initializeGame()
{
	// see requirements for the function from SpedenSpelit.h
  buttonNumber = -1;
  newTimerInterrupt = false;
  timerSpeedCounter = 0;
  timeToCheckGameStatus = false;
  for(int i=0;i<100;i++) {
    randomNumbers[i] = 0;
    userNumbers[i] = 0;
  }
  timerInterruptionCounter = 0;
  currentScore = 0;
  gameRunning = true;
}

void startTheGame()
{
   // see requirements for the function from SpedenSpelit.h
   initializeGame();
   sei(); //kts. initializeTimer() -funktion viimeinen rivi
}

void stopTheGame()
{
  cli();
  for(int i=0;i<100;i++) {
    randomNumbers[i] = 0;
    userNumbers[i] = 0;
  }
  gameRunning = false;
}
