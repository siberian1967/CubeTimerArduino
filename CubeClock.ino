#include <InvertedTM1638.h>
#include <TM1638.h>
#include <TM1638QYF.h>
#include <TM1640.h>
#include <TM16XX.h>
#include <TM16XXFonts.h>
#include <Bounce2.h>
#define BUTTON_PIN 13
Bounce debouncer = Bounce(); 

// define a module on data pin 8, clock pin 9 and strobe pin 7
TM1638 module(10, 9, 8);

// Debouncing variables
// Variables will change:
byte buttonState;             // the current reading from the input pin
byte lastButtonState = 0;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 200;    // the debounce time; increase if the output flickers

const int PIEZO_PIN = A0; // Piezo output

int sensorPin = 13; // select the input pin for the light binary
int sensorValue = 0; // variable to store the value coming from the sensor
int lastSensorValue = LOW ; // Track for trigger


void setup() {
      // put your setup code here, to run once:
    module.setDisplayToHexNumber(0x1234ABCD, 0xF0);
    module.setDisplayToString("CUBETIME");
    Serial.begin(9600);
    // API speed
    Serial.setTimeout(10);
    //pinMode(13, INPUT);
    pinMode(BUTTON_PIN,INPUT_PULLUP);
    debouncer.attach(BUTTON_PIN);
    debouncer.interval(5); // interval in ms
}

void loop() {
  // put your main code here, to run repeatedly:
  byte keys = module.getButtons();
  //module.setLEDs(((keys & 0xFF) << 8) | (keys & 0xFF));

  // light the first 4 red LEDs and the last 4 green LEDs as the buttons are pressed
  //module.setLEDs(((keys & 0xF0) << 8) | (keys & 0xF));

  byte buttons = module.getButtons();
 
  if (buttons != 0) {
    //Serial.println("PRESSED");
    if(notBouncing(buttons)){
      //Serial.println("Not bounced");
      emitButtonPress(buttons);
    }
  }
  debouncer.update();
  int value = debouncer.read();
  // Cube removed
  if ( value == HIGH  && value != lastSensorValue) {
    emitCubeUp();
  } 
  // Cube replaced
  else if ( value == LOW  && value != lastSensorValue) { 
    emitCubeDown();
  }
  lastSensorValue = value;

}

void serialEvent(){
  String data = Serial.readString();
  String command = data.substring(0,2);
  
  if (command == "sl") {
    int ledMask = data.substring(3).toInt();
    module.setLEDs((flipByte(ledMask) & 0xFF));
  } else if (command == "sm"){
    String message = data.substring(3);
    // Pad message out on right
    module.setDisplayToString(message);
  }
  //Serial.println(command);
}

bool notBouncing(byte buttons){
  //Serial.println(buttons);
  //Serial.println(lastButtonState);

  // If its a new button then its a new button, lets do this!
  if (buttons != lastButtonState) {
    //Serial.println("CASE 1");
    lastDebounceTime = millis();
    lastButtonState = buttons;
    return true;
  }

//  Serial.println(millis());
//  Serial.println(lastDebounceTime);
//  Serial.println(millis() - lastDebounceTime);
//  Serial.println((millis() - lastDebounceTime) > debounceDelay);
  
  // Ok, so is it a new button press on the existing button? Lets find out
    if ((millis() - lastDebounceTime) > debounceDelay) {
      //Serial.println("Its new");
      //Serial.println("CASE 2");
      lastDebounceTime = millis();
      return true;
    } else {
      //Serial.println("CASE 3");
      return false;
    }
 }

void emitCubeUp(){
  Serial.println("ce 1");
} 
void emitCubeDown(){
  Serial.println("ce 0");
}
void emitButtonPress(byte buttons){
  String button = "";
  Serial.print("bp ");
  Serial.println(buttons,DEC);
  return;
  
  // There is a more clever way to do this, it is base 2 after all. I am tired. Bitshifting etc  ((keys & 0xF0) << 8)
  if(buttons == 1 ) {
    button = "1";
  } else if(buttons == 2 ){
    button = "2";
  } else if(buttons == 4 ){
    button = "3";
  } else if(buttons == 8 ){
    button = "4";
  } else if(buttons == 16 ){
    button = "5";
  } else if(buttons == 32 ){
    button = "6";
  } else if(buttons == 64 ){
    button = "7";
  } else if(buttons == 128 ){
    button = "8";
  } 

  Serial.println("bp " + button);
}

byte flipByte(byte c)
     {
       c = ((c>>1)&0x55)|((c<<1)&0xAA);
       c = ((c>>2)&0x33)|((c<<2)&0xCC);
       c = (c>>4) | (c<<4) ;

       return c;
     }
/*
 * 
 * Some docs
 * 
 * https://github.com/rjbatista/tm1638-library/blob/master/examples/tm1638_one_module_example/tm1638_one_module_example.pde
 * https://www.arduino.cc/en/Tutorial/Debounce
 * 
 * /
 */

