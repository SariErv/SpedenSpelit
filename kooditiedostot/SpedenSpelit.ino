#include "display.h"
#include "buttons.h"
#include "leds.h"
#include "SpedenSpelit.h"

//#define DEFAULT_OCR_VALUE 15624

// Use these 2 volatile variables for communicating between
// loop() function and interrupt handlers
//volatile int pressedButton = -1;           // for buttons interrupt handler
volatile bool newTimerInterrupt = false;  // for timer interrupt handler

//muita pelissä tarvittavia muuttujia

//volatile int timerSpeedCounter = 0;   // for keeping track of increasing game speed
//unsigned long timeToCheckGameStatus = 0;

volatile bool timeToCheckGameStatus = false;

byte randomNumbers[100];
byte userNumbers[100];

int timerInterruptionCounter = 0;
int currentScore = 0;

bool gameRunning = false;

int tempIndex = 0; //keksi parempi nimi muuttujalle (while(gameRunning == false)-haarassa)

int buttonInterruptionsCounter = 0;

const int defaultOutCompRegValue = 15624; //vastaa yhden hertsin keskeytystaajuutta

int timerSpeed = defaultOutCompRegValue;

void setup()
{
  /*
    Initialize here all modules
  */
  Serial.begin(9600);
  initButtonsAndButtonInterrupts();
  initializeDisplay();
  showResult(currentScore);
  initializeLeds();
  //initializeTimer(timerSpeed);
  initializeGame();
  //startTheGame();
  Serial.println("Paina kahta oikeanpuoleista nappia perakkain aloittaaksesi pelin ...");
}

void loop()
{
  //odotetaan, että käyttäjä aloittaa pelin painamalla kahta oikeanpuoleista nappia peräkkäin
  while(!gameRunning)
  {
    //Serial.println("testi game running false");
    if(newTimerInterrupt)
    {
      flipAllLeds();
      newTimerInterrupt = false;
    }
    if(timeToCheckGameStatus)
    {
      Serial.print("time to check game status, pressed button "); //bugtest
      Serial.println(pressedButton);  //bugtest
      userNumbers[tempIndex] = pressedButton;
      tempIndex++;
      if((userNumbers[0] == 2 && userNumbers[1] == 3) || (userNumbers[0] == 3 && userNumbers[1] == 2))
      {
        Serial.println("userNum[0]=2 & userNum[1]=3 tai userNum[0]=3 ja userNum[1]=2"); //bugtest
        startTheGame();
        gameRunning = true; //sama koodirivi initializeGame()-funktiossa, jota startTheGame()-funktio kutsuu (ei siis tarvi tähän)
      }
      timeToCheckGameStatus = false;
      if(tempIndex > 1)
      {
        tempIndex = 0;
      }
    }
  }
  if(newTimerInterrupt)
  {
    // new random number must be generated
    // and corresponding let must be activated
    randomNumbers[timerInterruptionCounter] = random(0,4);  //arvo luku välillä 0-3 (byte)
    setLed(randomNumbers[timerInterruptionCounter]);
    timerInterruptionCounter++;
    if((timerInterruptionCounter % 10) == 0)
    {
      Serial.println("Peli nopeutuu!");
      timerSpeed = max(timerSpeed / 1.1, 0.2 * defaultOutCompRegValue); //nopeutetaan peliä 10%, mutta ei alle 200ms
      initializeTimer(timerSpeed);
    }
    newTimerInterrupt = false; 
  }
  if(timeToCheckGameStatus)
  {
    userNumbers[buttonInterruptionsCounter] = pressedButton;
    /*if(userNumbers[buttonInterruptionsCounter + 1] == 255)
    {
      clearAllLeds();
    }*/
    checkGame(pressedButton);
    buttonInterruptionsCounter++;
    timeToCheckGameStatus = false;
  }
  /*
  if((timerInterruptionCounter - buttonInterruptionsCounter) > 10)
  {
    showResult(currentScore);
    stopTheGame();
  }*/
}

void initializeTimer(int pelinNopeus)
{
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = pelinNopeus;
  TCCR1B |= B00001101;
  TIMSK1 |= B00000010;
  sei();
}

/*void initializeTimer()
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
  sei(); //set interrupts eli otetaan keskeytykset taas käyttöön
  Serial.println("timer alustettu");
}*/

ISR(TIMER1_COMPA_vect)
{
  /*
  Communicate to loop() that it's time to make new random number.
  Increase timer interrupt rate after 10 interrupts.
  */
  //timerSpeedCounter++;
  newTimerInterrupt = true;
  Serial.print("."); //bugitestaus
  /*if(timerSpeedCounter == 10) {
    OCR1A /= 1.1;   //lower the output compare register value, so that interrupts happen 10% more often
    timerSpeedCounter = 0;
  }*/
}

void checkGame(byte nbrOfButtonPush)
{
	// see requirements for the function from SpedenSpelit.h
  if(randomNumbers[currentScore] == nbrOfButtonPush)
  {
    Serial.println("Oikea nappi!");
    if(randomNumbers[currentScore+1] == 255)
    {
      clearAllLeds();
    }
    showResult(currentScore + 1);
    currentScore++;
    if(currentScore >= 99)
    {
      Serial.print("Onneksi olkoon, voitit pelin! Lopputulos: ");
      Serial.println(currentScore);
      clearAllLeds();
      stopTheGame();
    }
  }
  else 
  {
    Serial.print("Vaara nappi...");
    Serial.print(" Lopputulos: ");
    Serial.println(currentScore);
    clearAllLeds();
    stopTheGame();
  }
}

void initializeGame()
{
	// see requirements for the function from SpedenSpelit.h
  pressedButton = -1;
  newTimerInterrupt = false;
  //timerSpeedCounter = 0;
  timeToCheckGameStatus = false;
  for(int i=0;i<100;i++) {
    randomNumbers[i] = 255;
    userNumbers[i] = 255;
  }
  timerInterruptionCounter = 0;
  currentScore = 0;
  gameRunning = false;
  timerSpeed = defaultOutCompRegValue;
  initializeTimer(defaultOutCompRegValue);
}

void startTheGame()
{
   // see requirements for the function from SpedenSpelit.h
   clearAllLeds();
   initializeGame();
   showGO();
   delay(1000);
   showResult(currentScore);
   Serial.println("Peli alkaa!");
   gameRunning = true;
}

void stopTheGame()
{
  for(int i=0;i<100;i++) {
    randomNumbers[i] = 255;
    userNumbers[i] = 255;
  }
  gameRunning = false;
  Serial.println("Paina kahta oikeanpuoleista nappia perakkain aloittaaksesi uuden pelin");
}
