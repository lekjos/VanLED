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
bool writeToMem = false; 
int memWriteInterval = 1000;

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
float brightnessLED_pat = .1;
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
  int color_mode;
};

//PatternFade variables
int patternMode = 0;
int colorMode = 1; //color mode to control LED color
//vars to fade led
int prevFadeVal = 0;
int currentFadeVal = 0;
boolean increasing = true;
int fadeVal = 5; //value to step when increasing/decreasing, recommended to be 1 or 5, larger numbers will have problems lighting up
int fadeMAX = 255; //maximum fade value
int fadeMIN = 0;   //minimum fade value
int fadeDelay = 30;//delay between each step


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
  colorMode = customVar.color_mode;

  
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
  } else if (patternMode == 1) {
    brightnessLED = brightnessLED_pat;
    patternFade();
    
  }

  
//wait 1 second to write to EEPROM
  if (writeToMem == true){
    if (timeLastChange + memWriteInterval <= millis()){
      //write to memory
      #if DEBUG
        Serial.print("writing to EEPROM");
      #endif
      memStore customVar = {
        brightnessLED_white,
        brightnessLED_red,
        currentColor,
        colorMode
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

  redValue = int(redValue * brightnessLED);
  greenValue = int(greenValue * brightnessLED);
  blueValue = int(blueValue * brightnessLED);
  if (patternMode == 0) {
    if (blueValue <= 3 && blueValue != 0) {
      blueValue =3;
    }
  }
  //once value is calculated, show the LED color
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

void decreaseBrightness() { //red
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
  } else if (patternMode == 1) {
    if(brightnessLED_pat > (minLEDBrightness + fadeAmount)){
        brightnessLED_pat -= fadeAmount;
    }
  }
    
    #if DEBUG        
      Serial.print("brightness down ");
      Serial.println(brightnessLED);
    #endif
    delay(holdStepTime);
    timeLastChange = millis();
    writeToMem = true;
}

void increaseBrightness() {
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

  delay(holdStepTime);
  timeLastChange = millis();
  writeToMem = true;
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
    brightnessLED_pat = minLEDBrightness;
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

void patternFade() {

  switch (colorMode) {
    case 1://FADE RED
      redValue = currentFadeVal;
      greenValue = 0;
      blueValue = 0;

      rgbShow();
      break;
    //========== END FADE RED ==========

    case 2://FADE ORANGE
      redValue = currentFadeVal;
      greenValue = currentFadeVal * 0.498; // 128/255 = ~0.498039
      blueValue = 0;

      rgbShow();

      if (redValue > 0 && greenValue == 0) {
        //tertiary component is 1/2, so when it calculates to decimal with fade value,
        //it will be basically be off, make sure to turn off other color so that
        //it does not just show the other color
        redValue = 0;
      }
      // takes x amount of steps if you do not set it to zero for certain brightness (i.e. takes 8 more steps to turn off for 0.1)
      //Serial.print("Red Value =");
      //Serial.println( int((currentFadeVal) * brightnessLED));

      //Serial.print("Green Value =");
      //Serial.println( int((currentFadeVal * 0.498) * brightnessLED));
      break;
    //========== END FADE ORANGE ==========

    case 3://FADE YELLOW
      redValue = currentFadeVal;
      greenValue = currentFadeVal;
      blueValue = 0;

      rgbShow();
      break;
    //========== END FADE YELLOW ==========

    case 4://FADE CHARTRUESE
      redValue = currentFadeVal * 0.498; // 128/255 = ~0.498039
      greenValue = currentFadeVal;
      blueValue = 0;

      rgbShow();

      if (greenValue > 0 && redValue == 0) {
        //tertiary component is 1/2, so when it calculates to decimal with fade value,
        //it will be basically be off, make sure to turn off other color so that
        //it does not just show the other color
        greenValue = 0;
      }
      break;
    //========== END FADE CHARTRUESE ==========

    case 5://FADE GREEN
      redValue = 0;
      greenValue = currentFadeVal;
      blueValue = 0;

      rgbShow();
      break;
    //========== END FADE GREEN ==========

    case 6://FADE SPRING GREEN
      redValue = 0;
      greenValue = currentFadeVal;
      blueValue = currentFadeVal * 0.498; // 128/255 = ~0.498039

      rgbShow();

      if (greenValue > 0 && blueValue == 0) {
        //tertiary component is 1/2, so when it calculates to decimal with fade value,
        //it will be basically be off, make sure to turn off other color so that
        //it does not just show the other color
        greenValue = 0;
      }
      break;
    //========== END FADE SPRING GREEN ==========

    case 7://FADE CYAN
      redValue = 0;
      greenValue = currentFadeVal;
      blueValue = currentFadeVal;

      rgbShow();
      break;
    //========== END FADE CYAN ==========

    case 8://FADE AZURE
      redValue = 0;
      greenValue = currentFadeVal * 0.498; // 128/255 = ~0.498039
      blueValue = currentFadeVal;

      rgbShow();
      if (blueValue > 0 && greenValue == 0) {
        //tertiary component is 1/2, so when it calculates to decimal with fade value,
        //it will be basically be off, make sure to turn off other color so that
        //it does not just show the other color
        blueValue = 0;
      }
      break;
    //========== END FADE AZURE ==========

    case 9://FADE BLUE
      redValue = 0;
      greenValue = 0;
      blueValue = currentFadeVal;

      rgbShow();
      break;
    //========== END FADE BLUE ==========

    case 10://FADE VIOLET
      redValue = currentFadeVal * 0.498;
      greenValue = 0;
      blueValue = currentFadeVal;

      rgbShow();

      if (blueValue > 0 && redValue == 0) {
        //tertiary component is 1/2, so when it calculates to decimal with fade value,
        //it will be basically be off, make sure to turn off other color so that
        //it does not just show the other color
        blueValue = 0;
      }
      break;
    //========== END FADE VIOLET ==========

    case 11://FADE MAGENTA
      redValue = currentFadeVal;
      greenValue = 0;
      blueValue = currentFadeVal;

      rgbShow();
      break;
    //========== END FADE MAGENTA ==========

    case 12://FADE ROSE
      redValue = currentFadeVal;
      greenValue = 0;
      blueValue = currentFadeVal * 0.498;

      rgbShow();

      if (redValue > 0 && blueValue == 0) {
        //tertiary component is 1/2, so when it calculates to decimal with fade value,
        //it will be basically be off, make sure to turn off other color so that
        //it does not just show the other color
        redValue = 0;
      }
      break;
    //========== END FADE ROSE ==========

    case 13://FADE WHITE
      redValue = currentFadeVal;
      greenValue = currentFadeVal;
      blueValue = currentFadeVal;

      rgbShow();
      break;
    //========== END FADE WHITE ==========

    default:
      allOFF();
      rgbShow();
      break;
  }
  rgbShow();
  delay(fadeDelay);


  if (increasing == true) {
    currentFadeVal += fadeVal;
  }
  else { //decreasing
    currentFadeVal -= fadeVal;
  }

  if (currentFadeVal > fadeMAX) {
    increasing = false;
    prevFadeVal -= fadeVal;//undo addition

    currentFadeVal = prevFadeVal;

  }
  else if (currentFadeVal < fadeMIN) {
    increasing = true;
    prevFadeVal += fadeVal;//unto subtraction

    currentFadeVal = prevFadeVal;
  }

  prevFadeVal = currentFadeVal;
}//-------------------- END patternFade() FUNCTION --------------------
