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

volatile bool timeToCheckGameStatus = false;  /*buttons-moduulin ISR keskeytyspalvelin 
asettaa tämän muuttujan true, kun nappia painetaan.
pääohjelman void loop() tarkastaa joka kierroksella, onko muuttuja arvossa true.
*/

byte randomNumbers[100];  //lista, johon talletetaan arvotut numerot (ledien sytyttamiseen)
byte userNumbers[100];  //lista, johon talletetaan kayttajan painamien nappien numerot

int timerInterruptionCounter = 0; /*indeksimuuttuja, jonka arvoa kasvatetaan joka kerta, 
kun timer keskeytys tapahtuu. muuttujan avulla voidaan tallettaa 
randomNumbers[]-listaan peräkkäisiin alkioihin arvoja.
*/

int currentScore = 0; //pelin sen hetkinen tulos, joka näytetään 7-segmenttinäytöillä

bool gameRunning = false; /*muuttujan avulla voidaan ohjata, milloin peli on päällä, 
ja milloin peli on valmiustilassa. pääohjelman void loop() tarkistaa joka kierroksella,
onko gameRunning arvossa false. Niin kauan kun gameRunning == false, peli ei arvo ledejä
vaan on valmiustilassa ja odottaa käyttäjän aloittavan pelin
*/

int tempIndex = 0; //indeksimuuttuja, jota hyödynnetään tarkasteltaessa käyttäjän kahta viimeistä napinpainallusta.

int buttonInterruptionsCounter = 0; //muuttuja, jonka avulla pidetään laskua napinpainallusten lukumäärästä (voisi mahdollisesti muuttaa byte-tietotyypiksi?)

const int defaultOutCompRegValue = 15624; //vastaa yhden hertsin keskeytystaajuutta. (16 000 000 Hz / 1024) - 1 = 15624

int timerSpeed = defaultOutCompRegValue;  /*timerSpeed muuttujaa käytetään nopeuttamaan pelin nopeutta. 
timerSpeed annetaan initializeTimer()-funktiolle argumentiksi
*/

void setup()
{
  /*
    Initialize here all modules
  */
  Serial.begin(9600); //serial monitori
  initButtonsAndButtonInterrupts(); //alustetaan napit ja nappien keskeytyspalvelija
  initializeDisplay();  //alustetaan 7-segmenttinäyttö
  showResult(currentScore); //näytetään 7-segmenttinäytöllä tulos (tässä vaiheessa 0)
  initializeLeds(); //alustetaan ledit
  initializeGame(); //alustetaan pelissä käytettävät muuttujat
  Serial.println("Pelia alustetaan...");
  show2(5); //Kun ohjelma uploadataan Arduinolle, naytetaan ledeilla lyhyt valoshow
  Serial.println("Paina kahta oikeanpuoleista nappia perakkain aloittaaksesi pelin ...");
  Serial.println("Paina kahta vasemmanpuoleista nappia perakkain kaynnistaaksesi valoshow'n");
}

void loop()
{
  //odotetaan, että käyttäjä aloittaa pelin painamalla kahta oikeanpuoleista nappia peräkkäin
  while(!gameRunning) //peli ei ole päällä..
  {                   //tässä loopissa pyöritään, kunnes käyttäjä painaa kahta oikeanpuoleista nappia peräkkäin
    //Serial.println("testi game running false"); //bugitestausta
    if(timerInterruptionCounter > 30) //jos nappia ei ole painettu 30 sekuntiin, sammutetaan ledit ja display --> "lepotila"
    {
      timeToCheckGameStatus = false;
      clearAllLeds();
      clearDisplay();
      while(!timeToCheckGameStatus) //odota, kunnes nappia painetaan
      {

      } //arduino herää "lepotilasta" kun jotain nappia painetaan
      showResult(0);  //näytetään näytöllä tulos 0
      timerInterruptionCounter = 0;
    }
    if(newTimerInterrupt) //jokaisella timer keskeytyksellä (eli joka sekunti) kutsutaan flipAllLeds()-funktiota,
    {                     //joka "kääntää" kaikki ledit. Eli jos ledi on päällä, sammutetaan se. Jos ledi on kiinni, sytytetään se
      flipAllLeds();      //funktion määrittely leds-moduulissa
      newTimerInterrupt = false;  //if(x)-haaran lopussa muistettava muuttaa x takaisin false, muuten if-haaraan mennään joka kierroksella
      timerInterruptionCounter++; //30 sekunnin lepotila-ajastin
    }
    if(timeToCheckGameStatus) //jos nappia painettu, timeToCheckGameStatus on muuttunut arvoon true ja siirrytään tähän if-haaraan
    {
      timerInterruptionCounter = 0; //aloitetaan 30 sekunnin lepotila-ajastin uudestaan
      Serial.print("time to check game status, pressed button "); //bugtest
      Serial.println(pressedButton);  //bugtest
      timeToCheckGameStatus = false;  //if(x)-haaran lopussa muistettava muuttaa x takaisin false, muuten if-haaraan mennään joka kierroksella
      userNumbers[tempIndex] = pressedButton; //talletetaan käyttäjän painaman napin numero userNumbers-listaan, kohtaan tempIndex
      tempIndex++;  //lisätään tempIndex-muuttujaan +1, jotta seuraavalla kierroksella userNumbers-listaan talletettava napin numero ei kirjoita yli edellistä napin numeroa
      if((userNumbers[0] == 2 && userNumbers[1] == 3) || (userNumbers[0] == 3 && userNumbers[1] == 2)) //jos viimeiset kaksi napinpainallusta ovat 2 ja 3
      {
        Serial.println("kahta oikeanpuoleista nappia painettu"); //bugtest
        startTheGame();   //käynnistetään peli
        gameRunning = true; //seuraavalla kierroksella ei jäädä enää while(!gameRunning) haaraan
      }
      if((userNumbers[0] == 0 && userNumbers[1] == 1) || (userNumbers[0] == 1 && userNumbers[1] == 0))  //jos viimeiset kaksi napinpainallusta ovat 0 ja 1
      {
        Serial.println("kahta vasemmanpuoleista nappia painettu"); //bugitesti
        show1();
        //nollataan käytetyt listan arvot
        userNumbers[0] = 255;
        userNumbers[1] = 255;
      }
      if(tempIndex > 1)
      {
        tempIndex = 0;  //tempIndex oskilloi 0 ja 1 välillä (0,1,0,1,0,...), koska tarkastellaan vain kahta viimeistä napinpainallusta
      }                 //jos tarkasteltaisiin esimerkiksi kolmea viimeistä painallusta, muutettaisiin tempIndex oskilloimaan 0,1,2,0,1,2,...
    }
  }
  //käyttäjä on tässä vaiheessa aloittanut pelin
  if(newTimerInterrupt) //on kulunut yksi sekunti (pelin edetessä tämä aika vähenee)
  {                     //ja timer-keskeytyspalvelija on muuttanut newTimerInterrupt arvoon true
    // new random number must be generated
    // and corresponding let must be activated
    randomNumbers[timerInterruptionCounter] = random(0,4);  //arvo luku välillä 0-3 (byte) ja tallenna listaan randomNumbers, kohtaan timerInterruptionCounter
    //jos arvottu ledi on sama kuin edellinen, sammutetaan se hetkeksi (200ms)
    if(randomNumbers[timerInterruptionCounter] == randomNumbers[timerInterruptionCounter - 1])
    {
      clearAllLeds();
      delay(200);
    }
    setLed(randomNumbers[timerInterruptionCounter]);  //sytytetään arvottu ledi
    timerInterruptionCounter++;   //lisätään muuttujaan +1, jotta seuraavalla kierroksella ei kirjoiteta edellisen päälle
    if((timerInterruptionCounter % 10) == 0)  //kymmenen ledin välein nopeutetaan peliä 10%
    {
      Serial.println("Peli nopeutuu!");
      timerSpeed = max(timerSpeed / 1.1, 0.2 * defaultOutCompRegValue); //nopeutetaan peliä 10%, mutta ei alle 200ms
      initializeTimer(timerSpeed);
    }
    newTimerInterrupt = false; //if(x)-haaran lopussa muistettava muuttaa x takaisin false, muuten if-haaraan mennään joka kierroksella
  }
  if(timeToCheckGameStatus) //havaittu napinpainallus
  {
    userNumbers[buttonInterruptionsCounter] = pressedButton;  //talletetaan painetun napin numero userNumbers listaan kohtaan buttonInterruptionsCounter
    checkGame(pressedButton); //kutsutaan checkGame-funktiota, joka tarkistaa oliko painettu nappi oikein
    buttonInterruptionsCounter++; //lisätään muuttujaan +1, jotta seuraavalla kierroksella ei kirjoiteta edellisen päälle
    timeToCheckGameStatus = false;  //if(x)-haaran lopussa muistettava muuttaa x takaisin false, muuten if-haaraan mennään joka kierroksella
  }
  if((timerInterruptionCounter - buttonInterruptionsCounter) > 10)  //käyttäjä ei pysynyt pelin mukana ja jäi ainakin 10 lediä jälkeen
  {
    Serial.println("Painoit liian myohassa, havisit pelin!");
    showResult(currentScore); //näytetään 7-segmenttinäytöllä käyttäjän tulos
    clearAllLeds();   //sammutetaan kaikki ledit
    stopTheGame();    //pysäytetään peli
  }
}

void initializeTimer(int pelinNopeus) //timerin alustus
{
  cli(); //clear interrupts eli poistetaan keskeytykset käytöstä timerin alustuksen ajaksi
  //nollataan rekisterit (TCCR1A ja TCCR1B) sekä laskuri (TCNT1)
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  //asetetaan output compare rekisteri sellaiseen arvoon, jolla saadaan keskeytyksiä kerran sekunnissa
  OCR1A = pelinNopeus;  //ensimmäisellä funktion kutsukerralla pelinNopeus = defaultOutCompRegValue = 15624
  TCCR1B |= B00001101;  //WGM12 = 1, CS12 = 1, CS10 = 1 --> CTC-mode päälle, prescaleri = 1024
  TIMSK1 |= B00000010;  //OCIE1A = 1 --> output compare match A päälle
  sei();  //set interrupts eli otetaan keskeytykset taas käyttöön
  Serial.println("timer alustettu");
}

ISR(TIMER1_COMPA_vect)  //timerin keskeytyspalvelija
{
  /*
  Communicate to loop() that it's time to make new random number.
  Increase timer interrupt rate after 10 interrupts.
  */
  newTimerInterrupt = true; //muutetaan newTimerInterrupt arvoon true, jotta main loopissa havaitaan timer-keskeytyksen tapahtuneen
  Serial.print("."); //bugitestaus
}

void checkGame(byte nbrOfButtonPush)
{
	// see requirements for the function from SpedenSpelit.h
  //tarkistetaan, onko käyttäjän painama nappi sama kuin arvottu ledi
  if(randomNumbers[currentScore] == nbrOfButtonPush)  //jos nappi on oikein
  {
    Serial.println("Oikea nappi!");
    if(randomNumbers[currentScore+1] == 255)  //sammuttaa ledin nappia painettaessa, jos käyttäjä ei ole yhtäkään lediä jäljessä
    {
      clearAllLeds();
    }
    showResult(currentScore + 1); //näytetään 7-segmenttinäytöllä uusi tulos
    currentScore++; //lisätään tulokseen +1
    if(currentScore >= 99)  //peli päättyy, jos käyttäjä pääsee tulokseen 99
    {
      Serial.print("Onneksi olkoon, voitit pelin! Lopputulos: ");
      Serial.println(currentScore);
      clearAllLeds();
      stopTheGame();
    }
  }
  else //jos nappi on väärin
  {
    Serial.print("Vaara nappi...");
    Serial.print(" Lopputulos: ");
    Serial.println(currentScore);
    clearAllLeds();
    stopTheGame();
  }
}

void initializeGame() //alustetaan pelissä tarvittavat muuttujat sekä timer
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

void startTheGame() //käynnistää pelin
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

void stopTheGame()  //pysäytetään peli ja nollataan randomNumbers- sekä userNumbers-listat
{
  for(int i=0;i<100;i++) {
    randomNumbers[i] = 255;
    userNumbers[i] = 255;
  }
  gameRunning = false;
  timerInterruptionCounter = 0;
  Serial.println("Paina kahta oikeanpuoleista nappia perakkain aloittaaksesi uuden pelin");
  Serial.println("tai kahta vasemmanpuoleista nappia perakkain aloittaaksesi valoshow'n");
}
