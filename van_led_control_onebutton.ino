/*

*/
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
const float fadeAmount = 0.01;
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
  int color;
};

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
  buttonDown.setDebounceTicks(70);
  
//set last state
  memStore customVar;
  EEPROM.get(addr, customVar);
  brightnessLED_white = customVar.brightness_white;
  brightnessLED_red = customVar.brightness_red;
  currentColor = customVar.color;

  
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
  if (currentColor == 1){
    whiteON();
    brightnessLED = brightnessLED_white;
    rgbShow();
  }
  if (currentColor == 0) {
    redON();
    brightnessLED = brightnessLED_red;
    rgbShow();
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
        currentColor
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
  if (blueValue <= 3 && blueValue != 0) {
    blueValue =3;
  }
  //once value is calculated, show the LED color
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

void decreaseBrightness() { //red
    if (currentColor == 1){
      if(brightnessLED_white > (minLEDBrightness + fadeAmount)){
        brightnessLED_white -= fadeAmount;
     }
    } else { //red
      if(brightnessLED_red > (minLEDBrightness + fadeAmount)){
        brightnessLED_red -= fadeAmount;
     }      
    }
    
    #if DEBUG        
      Serial.print("brightness down ");
      Serial.println(brightnessLED);
    #endif
    delay(120);
    timeLastChange = millis();
    writeToMem = true;
}

void increaseBrightness() {
    
    if (currentColor == 1){
      if (brightnessLED_white < (maxLEDBrightness - fadeAmount)){
        brightnessLED_white += fadeAmount;
      }
    } else {
       if (brightnessLED_red < (maxLEDBrightness - fadeAmount)){
        brightnessLED_red += fadeAmount;   
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

  delay(120);
  timeLastChange = millis();
  writeToMem = true;
}

void colorChange() {
  Serial.print("start color: ");
  Serial.println(currentColor);
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



void minBrightness() {
  if (currentColor == 1){ //white
    brightnessLED_white = minLEDBrightness;
  } else {
    brightnessLED_red = minLEDBrightness;
  }
  
  timeLastChange = millis();
  writeToMem = true;
}

void maxBrightness() {
   if (currentColor
   == 1){ //white
    brightnessLED_white = maxLEDBrightness;
  } else {
    brightnessLED_red = maxLEDBrightness;
  } 

  timeLastChange = millis();
  writeToMem = true;
}
