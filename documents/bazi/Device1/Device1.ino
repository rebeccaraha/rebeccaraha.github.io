#include <math.h>

#include "neopixel.h"
#define PIXEL_PIN D5
#define PIXEL_COUNT 16
#define PIXEL_TYPE WS2812

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

int redButton = D0;
int yellowButton = D1;
int greenButton = D2;
int blueButton = D3;
int greyButton = D4;

int vibrationMotor = D7;

int redButtonState = 0;
int yellowButtonState = 0;
int greenButtonState = 0;
int blueButtonState = 0;
int greyButtonState = 0;

int changingMode = 0;
int firstTime = 0;

int val = 0;

int colorCycle = 0;

int buffer[16] = {0};
int in = 0;
int out = 0;
int bufferLen = 0;

// OP stands for other player
int bufferOP[16] = {0};
int bufferOPLen = 0;

int returnPlayLen = 0;
int OPReturnTurns = 0;

String player1btns = "";
String str ="";
String player1status = "";

int pressIndex = 0;

int systemOnColor = 0;

volatile byte half_revolutions;
volatile int turnOn = 0;

int deviceStatus = 0;

int waitingForYouToPress = 0;

void setup() {
  pinMode(redButton, INPUT_PULLUP);
  pinMode(yellowButton, INPUT_PULLUP);
  pinMode(greenButton, INPUT_PULLUP);
  pinMode(blueButton, INPUT_PULLUP);
  pinMode(greyButton, INPUT_PULLUP);

  pinMode(vibrationMotor, OUTPUT);

  strip.begin();
  strip.show();

  Serial.begin (9600);
  //Subscribe to other player
  Particle.subscribe("raha/player2btns", player2btns); //subscribing to other player
  Particle.subscribe("raha/player2status", player2status);
//  attachInterrupt(D6, magnetDetect, RISING); //Initializing hall effect interrupt
  //Rising means sensor is moving away from the magnet (device away from base)

  changeColor(strip.Color(0,0,0));
}

void loop() {
  redButtonState = digitalRead(redButton);
  yellowButtonState = digitalRead(yellowButton);
  greenButtonState = digitalRead(greenButton);
  blueButtonState = digitalRead(blueButton);
  greyButtonState = digitalRead(greyButton);

  if (systemOnColor == 1){ systemOn(); }

  switch (deviceStatus) {
    case 1:
      wantToPlay();
      break;
    case 2:
      waitingToBegin();
      break;
    case 3:
      letsPlay();
      break;
    case 4:
      waitingBetweenTurns();
      break;
    case 5:
      OPTurn();
      break;
    case 6:
      systemOff();
      break;
    case 7:
      winner(50);
      break;
    case 8:
      playWithMe();
      break;
  }
  checkSystem();

  /*if (waitingForYouToPress == 1) {
    buzz();
  }*/
}
/*
void buzz() {
  digitalWrite(vibrationMotor, HIGH);
  delay(300);
  digitalWrite(vibrationMotor, LOW);
  delay(1000);
}*/

void playWithMe() {
  colorCycle++;
  digitalWrite(vibrationMotor, HIGH);
  if (colorCycle == 1) {
    changeColor(strip.Color(255,0,0));
  }

  else if (colorCycle == 2) {
    changeColor(strip.Color(255, 150, 0));
  }
  else if (colorCycle == 3) {
    changeColor(strip.Color(0, 255, 0));
  }

  else if (colorCycle == 4) {
    changeColor(strip.Color(0, 0, 255));
    colorCycle = 0;
  }
  delay(300);
  digitalWrite(vibrationMotor, LOW);
  changeColor(strip.Color(0,0,0));
  delay(500);
}

void waitingBetweenTurns() {
  for(int s = 0; s <= 250; s += 10) {
    changeColor(strip.Color(s,s,s));
    delay(30);
  }
  delay(250);
  for(int i = 240; i >= 0; i -= 10) {
    changeColor(strip.Color(i,i,i));
    delay(30);
  }
  changeColor(strip.Color(0,0,0));
  delay(1000);
}

void wantToPlay(){
  player1status = "play";
  publishStatus();
  //This is good I think
  deviceStatus = 2;
}

void waitingToBegin() {
  breathe();
}

void systemOn() {
  systemOnColor = 0;
  Serial.print("System On");
  Serial.println();
  for(int i = 0; i <= 255; i += 15) {
    changeColor(strip.Color(i,i,i));
    delay(50);
  }

  delay(500);

  for(int i = 250; i >= 0; i -= 15) {
        changeColor(strip.Color(i,i,i));
        delay(5);
  }
  for(int i = 0; i <= 255; i += 15) {
    changeColor(strip.Color(i,i,i));
    delay(5);
  }
  delay(100);
  changeColor(strip.Color(0,0,0));
}

//Records button presses as they happen, sets val to button number and adds to array
void letsPlay () {
  if (changingMode == 0){
    vibrate();
  }
  changeColor(strip.Color(0,0,0));
  if (redButtonState == LOW) { val = 1; }
  else if (yellowButtonState == LOW) { val = 2; }
  else if (greenButtonState == LOW) { val = 3; }
  else if (blueButtonState == LOW){ val = 4; }
  else { val = 0; }

  flash(val);

  changingMode = 1;

  //Makes sure a number is being added - not a 0
  if (val > 0) {
    enqueue(val);
    waitingForYouToPress = 0;
  }
}

void flash(int color) {
  /*Serial.print("void flash()");
  Serial.println();*/
  switch (color) {
    case 1:
    	changeColor(strip.Color(255, 0, 0));
      delay(300);
    	break;
    case 2:
      changeColor(strip.Color(255, 150, 0));
      delay(300);
    	break;
    case 3:
      changeColor(strip.Color(0, 255, 0));
      delay(300);
    	break;
    case 4:
    	changeColor(strip.Color(0, 0, 255));
      delay(300);
    	break;
    default:
      changeColor(strip.Color(0,0,0));
    	break;
  }

  if (color > 0) {
    Serial.print("flash color: ");
    Serial.print(color);
    Serial.println();
  }
  changeColor(strip.Color(0, 0, 0));
  delay(100);
}

void slowerFlash(int color) {
  /*Serial.print("void flash()");
  Serial.println();*/
  switch (color) {
    case 1:
    	changeColor(strip.Color(255, 0, 0));
      delay(500);
    	break;
    case 2:
      changeColor(strip.Color(255, 150, 0));
      delay(500);
    	break;
    case 3:
      changeColor(strip.Color(0, 255, 0));
      delay(500);
    	break;
    case 4:
    	changeColor(strip.Color(0, 0, 255));
      delay(500);
    	break;
    default:
      changeColor(strip.Color(0,0,0));
    	break;
  }

  if (color > 0) {
    Serial.print("flash color: ");
    Serial.print(color);
    Serial.println();
  }
  changeColor(strip.Color(0, 0, 0));
  delay(300);
}


void player2btns(const char *event,const char *data) {
  Serial.print("void player2btns()");
  Serial.println();
  //this is the handler for receiving the subscribed event
    String newPatternEvent = String(event);
    String newPatternData = String(data);

    int currentOffset = 0;
    int bufferIndex = 0;
    int commaPos = 0;
    bufferOPLen = 0;

    while( (newPatternData.indexOf( ",", currentOffset )) != -1 ) {
      //Finds index of the comma (what you're looking for, where you start)
      commaPos = newPatternData.indexOf(",", currentOffset);
      String valueStr;
      //Finds substring between commas
      valueStr = newPatternData.substring(currentOffset, commaPos);
      int value = valueStr.toInt();

      if (value > 0) {
        bufferOP[bufferIndex] = value;
        bufferIndex++;
        bufferOPLen++;
      }
      currentOffset = commaPos + 1;
    }

    changingMode = 0;

    if (changingMode == 0){
      vibrate();
      changingMode = 1;
    }

  for(int i = 0; i < bufferOPLen; i++){ //Reads buffer
    //sets btn to each item in the buffer and plays the corresponding case number
    int btn = bufferOP[i];
    Serial.print("flash color from player2btns");
    Serial.println();
    slowerFlash(btn);

    Serial.print("color: ");
    Serial.print(btn);
  }

  deviceStatus = 5;
}

void player2status(const char *event,const char *data) {
  Serial.print("void player2status()");
  Serial.println();
  String newPlayer2StatusEvent = String(event);
  String newPlayer2StatusData = String(data);

  if (newPlayer2StatusData == "lost") {
      deviceStatus = 7;
    }

  else if (newPlayer2StatusData == "play") {
    deviceStatus = 3;
    waitingForYouToPress = 1;
  }

    Serial.print("player status is ");
    Serial.println(newPlayer2StatusData);
}

void OPTurn(){
  /*Serial.print("void OPTurn()");
  Serial.println();*/
   if (redButtonState == LOW) { val = 1; }
   else if (yellowButtonState == LOW) { val = 2; }
   else if (greenButtonState == LOW) { val = 3; }
   else if (blueButtonState == LOW){ val = 4; }
   else { val = 0; }

   if (val > 0)  {
    Serial.print("about to call flash val from OPTurn");
    Serial.println();
    flash(val);

     if (val == bufferOP[pressIndex]) { //Makes sure a number is being added - not a 0
       pressIndex++;
       OPReturnTurns++;
      //Game Continues
       if (pressIndex == bufferOPLen) { //this has to happen
         clearBuffer(); //was originally clear everything
          completedSuccessfully(strip.Color(100,100,100), 50);
          changeColor(strip.Color(0,0,0));
          deviceStatus = 3;
          changingMode = 0;
	  	  }
     }
    	else if (val != bufferOP[pressIndex]) {
         pressIndex = bufferOPLen;
         loser(strip.Color(0, 50, 200));
         deviceStatus = 6;
       }
   }
}

void vibrate () {
  digitalWrite(vibrationMotor, HIGH);
  delay(1000);
  digitalWrite(vibrationMotor, LOW);
  delay(300);
  digitalWrite(vibrationMotor, HIGH);
  delay(1000);
  digitalWrite(vibrationMotor, LOW);
  delay(300);
}

void completedSuccessfully(uint32_t c, uint8_t wait) {
  val = 0;
  Serial.print("void completedSuccessfully()");
  Serial.println();
  delay(500);
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
  colorWipe(strip.Color(0, 0, 0), 125);
  clearBuffer();
  deviceStatus = 3;
}

void checkSystem() {
  /*Serial.print("void checkSystem()");
  Serial.println();*/
  if (greyButtonState == LOW) {
    if (changingMode == 0) {
      systemOnColor = 1;
      deviceStatus = 1;
    }
    else if (changingMode == 1) {
      for(int i = 0; i < bufferLen; i++){ //Reads buffer
        //sets btn to each item in the buffer and plays the corresponding case number
        int btn = buffer[i];
        Serial.print("flash color from checksystem");
        Serial.println();
        flash(btn);
        Serial.print("flash: ");
        Serial.print(btn);
        Serial.println();
      }
      getBuffer();
    }
  }
}

//String for getbuffer
void getBuffer(){
  Serial.print("void getBuffer()");
  Serial.println();
  for(int i = 0; i < bufferLen; i++){
    str = str + String( buffer[i] );
    str = str + ",";
  }
  player1btns = str;
  Serial.print("final string: ");
  Serial.print(player1btns);
  Particle.publish("raha2/player1btns", player1btns);
  clearBuffer();
  str = "";

  deviceStatus = 4;
}

//NEOPIXEL STUFF
void changeColor(uint32_t color) {
  for(uint16_t i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
  }
}

void loser(uint32_t c) {
  Serial.print("void loser()");
  Serial.println();

  player1status = "lost";
  publishStatus();

  for(int i=0; i< strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
  delay(100);

  colorWipe(strip.Color(0, 0, 0), 125);

  deviceStatus = 6;
}

void publishStatus() {
  Particle.publish("raha2/player1status", player1status);
}

void winner(uint8_t wait) {
  Serial.print("void winner()");
  Serial.println();
  for (int j=0; j < 256; j = j + 15) {     // cycle all 256 colors in the wheel
     for (int q=0; q < 3; q++) {
       for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
         strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
       }
       strip.show();

       delay(wait);

       for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
         strip.setPixelColor(i+q, 0);        //turn every third pixel off
     }
    }
  }
  changeColor(strip.Color(0,0,0));
  deviceStatus = 6;
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

//BUFFER RELATED
void enqueue(int value) { /* Add one item to the front of the queue*/
  buffer[in] = value;
  in++; /* Advance the index for next time */
  if (in == 16) in = 0; /* Wrap around! */
  bufferLen++;
}

void enqueueOP(int value) { /* Add one item to the front of the queue*/
  buffer[in] = value;
  in++; /* Advance the index for next time */
  if (in == 16) in = 0; /* Wrap around! */
  returnPlayLen++;
}

//Not called yet, will be needed later
void clearBuffer() {
	for (int i = 0; i < bufferLen; i++){
		buffer[i] = 0;
	}
  bufferLen = 0;
  buffer[16] = {0};
  bufferLen = 0;
  bufferOP[16] = {0};
  bufferOPLen = 0;

  player1btns = "";
  str ="";
  pressIndex = 0;
  in = 0;
  out = 0;
}

void systemOff () {
  buffer[16] = {0};
  bufferLen = 0;
  bufferOP[16] = {0};
  bufferOPLen = 0;

  player1btns = "";
  str ="";
  pressIndex = 0;
}

void magnetDetect() {//This function is called whenever a magnet/interrupt is detected by the arduino
  half_revolutions++;
  Serial.print("detect");
  Serial.println();
  systemOnColor = 1;
  deviceStatus = 1;
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

float breathe( ){

  float val = (exp(sin(millis()/2000.0*M_PI)) - 0.36787944)*108.0;

  uint16_t i;
  uint32_t c = strip.Color(val, val, val);

  for(i=0; i< strip.numPixels(); i++) {
    strip.setPixelColor(i, c );
  }
  strip.show();

}
