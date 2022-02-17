/*
This program controls the LEDs in Stan the Van. There are three buttons and a single RGB LED channel.
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <OneButton.h>

//Debug mode, comment one of these lines out using a syntax
//for a single line comment ("//"):
#define DEBUG 0     //0 = LEDs only
//#define DEBUG 1     //1 = LEDs w/ serial output

//memory vars
int addr = 0;  // eeprom address
unsigned long timeLastChange = 0;
unsigned long timeLastHold = 0;
bool writeToMem = false; 
int memWriteInterval = 10000;

//define led output pins
#define redPin 3
#define greenPin 9
#define bluePin 11
#define brightnessUpPin 8
#define brightnessDownPin 12
#define colorChangePin 7



OneButton buttonUp(brightnessUpPin, true);
OneButton buttonDown(brightnessDownPin, true);
OneButton buttonColor(colorChangePin, true);


//Create brightness variable
//Ranging from 0.0-1.0:
//  0.0 is off
//  0.5 is 50%
//  1.0 is fully on
float brightnessLED = .1;
float brightnessLED_white = .1;
float brightnessLED_red = .1;
float brightnessLED_pat = .5;
const float fadeAmount = 0.01;
const int holdStepTime = 45; //ms
const float minLEDBrightness = 0.05;
const float maxLEDBrightness = 1.0;

// Create integer variables for our LED color value
int redValue = 0;
int greenValue = 0;
int blueValue = 0;
int currentColor = 0;


struct memStore {
  float brightness_white;
  float brightness_red;
  float brightness_pat;
  int color;
  int rainTransition;
  int rainRedVal;
  int rainGreenVal;
  int rainBlueVal;
};

//PatternFade variables
int patternMode = 0;

//Variables to transition between RGB in a rainbow
int rainbowRedVal = 0;
int rainbowGreenVal = 0;
int rainbowBlueVal = 0;
int rainbowStep = 1;
int rainbowTransitionVal = 0;
int rainbowDelay = 75; //in milliseconds to transition between colors
unsigned long timeLastRainbow = 0;

void setup() {
  // initialize serial communication at 9600 bits per second:
  #if DEBUG 
    Serial.begin(9600);  
  #endif

//set pwm clock for pin 3 to match other pins
//  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
//  TCCR2B = _BV(CS22);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);


//brightness up button setup  
  buttonUp.attachDoubleClick(maxBrightness);
  buttonUp.attachDuringLongPress(increaseBrightness);
  buttonUp.setPressTicks(800);
  buttonUp.setDebounceTicks(70);

//brightness down button setup
  buttonDown.attachDoubleClick(minBrightness);
  buttonDown.attachDuringLongPress(decreaseBrightness);
  buttonDown.setPressTicks(800);
  buttonDown.setDebounceTicks(70);
      
//color button setup
  buttonColor.attachClick(colorChange);
  buttonColor.attachDoubleClick(changePattern);
  buttonDown.setDebounceTicks(70);
  
//set last state
  memStore customVar;
  EEPROM.get(addr, customVar);
  brightnessLED_white = customVar.brightness_white;
  brightnessLED_red = customVar.brightness_red;
  brightnessLED_pat = customVar.brightness_pat;
  currentColor = customVar.color;
  rainbowTransitionVal = customVar.rainTransition;
  rainbowRedVal = customVar.rainRedVal;
  rainbowGreenVal = customVar.rainGreenVal;
  rainbowBlueVal = customVar.rainBlueVal;


  //set current time as last written time
  timeLastChange = millis();

}
  
void loop(){

//get button states
  buttonUp.tick();
  delay(5);
  buttonDown.tick();
  delay(5);
  buttonColor.tick();
  delay(5);

//Turn on LEDs based on current color
  if (patternMode == 0) {
    if (currentColor == 1){
      whiteON();
      brightnessLED = brightnessLED_white;
      rgbShow();
    } else if (currentColor == 0) {
      redON();
      brightnessLED = brightnessLED_red;
      rgbShow();
    }
  } else if (patternMode == 1) { // if in rainbow mode
    brightnessLED = brightnessLED_pat;
    rainbowFade();
    
  }

  
//wait 10 seconds to write to EEPROM
  if (writeToMem == true){
    if (timeLastChange + memWriteInterval <= millis()){
      //write to memory
      #if DEBUG
        Serial.print("writing to EEPROM");
      #endif
      memStore customVar = {
        brightnessLED_white,
        brightnessLED_red,
        brightnessLED_pat,
        currentColor,
        rainbowTransitionVal,
        rainbowRedVal,
        rainbowGreenVal,
        rainbowBlueVal
      };
      EEPROM.put(addr, customVar);
      writeToMem = false;
    }
  }


}//end loop

// ==================== CUSTOM FUNCTIONS DEFINED BELOW ====================
void allOFF() {
  // Black (all LEDs off)
  // RGB LEDs:
  redValue = 0;
  greenValue =  0;
  blueValue = 0;
}

void redON() {
  // Red
  redValue = 255;
  greenValue =  0;
  blueValue = 0;
}

void orangeON() {
  // Orange
  redValue = 255;
  greenValue = 128;
  blueValue = 0;
}

void yellowON() {
  // Yellow
  redValue = 255;
  greenValue = 255;
  blueValue = 0;
}

void chartrueseON() {
  // Chartruese
  redValue = 128;
  greenValue = 255;
  blueValue = 0;
}

void greenON() {
  // Green
  redValue = 0;
  greenValue = 255;
  blueValue = 0;
}

void springGreenON() {
  // Spring Green
  redValue = 0;
  greenValue = 255;
  blueValue = 128;
}

void cyanON() {
  // Cyan
  redValue = 0;
  greenValue = 255;
  blueValue = 255;
}

void azureON() {
  // Azure
  redValue = 0;
  greenValue = 128;
  blueValue = 255;
}

void blueON() {
  // Blue
  redValue = 0;
  greenValue = 0;
  blueValue = 255;
}

void violetON() {
  // Violet
  redValue = 128;
  greenValue = 0;
  blueValue = 255;
}

void magentaON() {
  // Magenta
  redValue = 255;
  greenValue = 0;
  blueValue = 255;
}

void roseON() {
  // Rose
  redValue = 255;
  greenValue = 0;
  blueValue = 128;
}

void whiteON() {
  // White (all LEDs on)
  redValue = 255;
  greenValue = 140;
  blueValue = 30;
}



void rgbShow() {
if (patternMode == 0) {
    redValue = int(redValue * brightnessLED);
    greenValue = int(greenValue * brightnessLED);
    blueValue = int(blueValue * brightnessLED);
    if (blueValue <= 3 && blueValue != 0) {
      blueValue =3;
    }
  } //else if patternMode == 1, do just show the values

  //once value is calculated, show the LED color
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

void decreaseBrightness() { //red
  if (timeLastHold + holdStepTime <= millis()){
    if (patternMode == 0){
      if (currentColor == 1){
        if(brightnessLED_white > (minLEDBrightness + fadeAmount)){
          brightnessLED_white -= fadeAmount;
      }
      } else { //red
        if(brightnessLED_red > (minLEDBrightness + fadeAmount)){
          brightnessLED_red -= fadeAmount;
      }      
      }
    } else if (patternMode == 1) { // rainbow
      if(brightnessLED_pat > (minLEDBrightness + fadeAmount)){
          brightnessLED_pat -= fadeAmount;
      }
    }
      
      #if DEBUG        
        Serial.print("brightness down ");
        Serial.println(brightnessLED);
      #endif
      timeLastChange = millis();
      timeLastHold = timeLastChange;
      writeToMem = true;
  }
}

void increaseBrightness() {
  if (timeLastHold + holdStepTime <= millis()){
    if (patternMode == 0){
      if (currentColor == 1){
        if (brightnessLED_white < (maxLEDBrightness - fadeAmount)){
          brightnessLED_white += fadeAmount;
        }
      } else {
        if (brightnessLED_red < (maxLEDBrightness - fadeAmount)){
          brightnessLED_red += fadeAmount;   
        }  
      }
    } else if (patternMode == 1) {
      if (brightnessLED_pat < (maxLEDBrightness - fadeAmount)){
          brightnessLED_pat += fadeAmount;   
        } 
    }

        #if DEBUG
          Serial.print("brightness up ");
          Serial.println(brightnessLED);
          Serial.print("R: ");
          Serial.print(redValue);
          Serial.print("G: ");
          Serial.print(greenValue);
          Serial.print("B: ");
          Serial.println(blueValue);
        #endif 
    timeLastChange = millis();
    timeLastHold = timeLastChange;
    writeToMem = true;
  }
}

void colorChange() {
  #if DEBUG
    Serial.print("start color: ");
    Serial.println(currentColor);
  #endif

  if (patternMode == 1){ // if pattern is on, shut it off
    patternMode = 0;
  } else { // otherwise, change color
    if (currentColor == 1){ //if white change to red
      currentColor=0;
      }  else {
      currentColor=1;  //if red change to white
      }
      timeLastChange = millis();
      writeToMem = true;

      #if DEBUG
        Serial.print("Color Changed to: ");
        Serial.println(currentColor);
      #endif
  }
}

void minBrightness() {
  if (patternMode == 0){
    if (currentColor == 1){ //white
      brightnessLED_white = minLEDBrightness;
    } else {
      brightnessLED_red = minLEDBrightness;
    }
  } else if (patternMode == 1){
    brightnessLED_pat = minLEDBrightness;
  }
  timeLastChange = millis();
  writeToMem = true;
}

void maxBrightness() {
  if (patternMode == 0){
    if (currentColor
    == 1){ //white
      brightnessLED_white = maxLEDBrightness;
    } else { //red
      brightnessLED_red = maxLEDBrightness;
    } 
  } else if (patternMode == 1){ //fade
    brightnessLED_pat = maxLEDBrightness;
  }

  timeLastChange = millis();
  writeToMem = true;
}

void changePattern(){
  // change to or from pattern on double click
  if (patternMode == 1){ 
  patternMode=0;
  }  else {
  patternMode=1;  
  }
}


void rainbowFade(){
  if (timeLastRainbow + rainbowDelay <= millis()){
      if (rainbowTransitionVal == 0) {
      //RED
      rainbowRedVal -= rainbowStep;
      if (rainbowRedVal <= 0) {
        rainbowTransitionVal = 1;
      }
    }
    else if (rainbowTransitionVal == 1) {
      //RED TO ORANGE TO YELLOW
      rainbowGreenVal -= rainbowStep;

      if (rainbowGreenVal <= 0) {
        rainbowTransitionVal = 2;
      }
    }
    else if (rainbowTransitionVal == 2) {
      //YELLOW to CHARTREUSE to GREEN
      rainbowRedVal += rainbowStep;

      if (rainbowRedVal >= 255) {
        rainbowTransitionVal = 3;
      }
    }
    else if (rainbowTransitionVal == 3) {
      //GREEN to SPRING GREEN to CYAN
      rainbowBlueVal -= rainbowStep;

      if (rainbowBlueVal <= 0) {
        rainbowTransitionVal = 4;
      }
    }
    else if (rainbowTransitionVal == 4) {
      //CYAN to AZURE to BLUE
      rainbowGreenVal += rainbowStep;

      if (rainbowGreenVal >= 255) {
        rainbowTransitionVal = 5;
      }
    }
    else if (rainbowTransitionVal == 5) {
      //BLUE to VIOLET to MAGENTA
      rainbowRedVal -= rainbowStep;

      if (rainbowRedVal <= 0) {
        rainbowTransitionVal = 6;
      }
    }
    else if (rainbowTransitionVal == 6) {
      //MAGENTA to ROSE to RED
      rainbowBlueVal += rainbowStep;


      if (rainbowBlueVal >= 255) {
        rainbowTransitionVal = 1;
      }
    }
  redValue = int(rainbowRedVal * brightnessLED);
  greenValue = int(rainbowGreenVal * brightnessLED);
  blueValue = int(rainbowBlueVal * brightnessLED);

  rgbShow();
  timeLastRainbow = millis();
  } // end last update conditional
  

}
