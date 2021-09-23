/* Shiftlight with CAN-BUS for MQB */
/* Board: Arduino Nano
 * Processor: ATmega328P (Old Bootloader)
 * Port: /dev/cu.usbserial-1410
 */


#include <Adafruit_NeoPixel.h>
#include <mcp_can.h>
#include <SPI.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

//pin config
#define PINPIXELS 3 // pin D3 to data-in WS2812 module
#define NUMPIXELS 8 // number of LEDs in WS2812 module
#define PINBUZZER 4 // pin D2 to pizo speaker
#define GNDBUZZER 5 // ground for pizo speaker
#define CAN0_INT 2 // MCP2515 INT to pin 2

//MCP2515 config
MCP_CAN CAN0(10); // MCP2515 CS to pin 10
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128]; 

//initial config
Adafruit_NeoPixel pixels(NUMPIXELS, PINPIXELS, NEO_GRB + NEO_KHZ800);
unsigned int initialBrightness = 15; // 0-255
unsigned int currentBrightness = initialBrightness;

unsigned int currentFrequency = 500;

//user config
unsigned int currentRPM;
unsigned int maxRPM = 8000;
unsigned int perfectRPM = 6500;
unsigned int welcomeMode = 2;
bool mute = true;

unsigned long nextLoopTime=0;

//run once
void setup() {
  //Serial communication setup
  Serial.begin(115200);
  
  //First data output over serial
  printInitialDetails();

  ///MCP2515 setup
  mcpSetup();

  //WS2812 setup
  pixels.begin();
  pixels.clear();
  pixels.show();
  pixels.setBrightness(currentBrightness);

  //Buzzer setup
  pinMode(PINBUZZER, OUTPUT);
  pinMode(GNDBUZZER, OUTPUT);
  digitalWrite(GNDBUZZER, LOW);

  //Run initial animation
  welcomeCeremony(welcomeMode);
}

//run all the time
void loop() {
  mcpRead();
  //mcpSearch();
    
  currentRPM = getRPM(false); //false = simulation
  
  pixels.clear();
  pixels.setBrightness(currentBrightness);

  updateAlerts();

  pixels.show();
}

void welcomeCeremony(int type) {
  if(type==1){
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 0, 0));
      pixels.show();
      tone(PINBUZZER, (currentFrequency*(i+1)));
      delay(25);
      noTone(PINBUZZER);
      delay(25);
    }
  }

  
  if(type==2){
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.clear();
      pixels.setPixelColor(i, pixels.Color(0, 0, 255));
      pixels.show();
      tone(PINBUZZER, (currentFrequency*(i+1)));
      delay(25);
    }
    for (int i=7; i>=0; i--) {
      pixels.clear();
      pixels.setPixelColor(i, pixels.Color(0, 0, 255));
      pixels.show();
      tone(PINBUZZER, (currentFrequency*(i+1)));
      delay(25);
    }
    noTone(PINBUZZER);

  }
}

int getRPM(bool real){
  if(real){
    //read RPM from CAN-BUS
    //TODO
  }else{
    //simulate RPMs for development and debug
    return millis()%maxRPM;
  }
}

void updateAlerts(){
  if(currentRPM<(perfectRPM-1000)){
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
  }

  if(currentRPM>=(perfectRPM-1000) && currentRPM<(perfectRPM-800)){
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.setPixelColor(7, pixels.Color(0, 255, 0));
  }

  if(currentRPM>=(perfectRPM-800) && currentRPM<(perfectRPM-600)){
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.setPixelColor(1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(6, pixels.Color(0, 255, 0));
    pixels.setPixelColor(7, pixels.Color(0, 255, 0));
  }

  if(currentRPM>=(perfectRPM-600) && currentRPM<(perfectRPM-400)){
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.setPixelColor(1, pixels.Color(0, 255, 0));
    pixels.setPixelColor(2, pixels.Color(0, 255, 0));
    pixels.setPixelColor(5, pixels.Color(0, 255, 0));
    pixels.setPixelColor(6, pixels.Color(0, 255, 0));
    pixels.setPixelColor(7, pixels.Color(0, 255, 0));
  }

  if(currentRPM>=(perfectRPM-400) && currentRPM<(perfectRPM-200)){
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 255, 0));
    }
  }
  
  if(currentRPM>=(perfectRPM-200) && currentRPM<(perfectRPM+200)){
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 255));
    }
  }

  if(currentRPM>=(perfectRPM+200) && currentRPM<(perfectRPM+500)){
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    }
  }

  if(currentRPM>=(perfectRPM+500)){
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    }
    pixels.show();
    if(!mute){
      tone(PINBUZZER, currentFrequency*8);
    }
    delay(50);
    pixels.clear();
    pixels.show();
    noTone(PINBUZZER);
    delay(50);
  }
}

void mcpSetup(){
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK){
    Serial.println("MCP2515 Initialized Successfully!");
  }else{
    Serial.println("Error Initializing MCP2515...");
  }
  CAN0.setMode(MCP_NORMAL); // Set operation mode to normal so the MCP2515 sends acks to received data.
  pinMode(CAN0_INT, INPUT); // Configuring pin for /INT input
  printSpacer();
}

void mcpRead(){
  if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
    
    if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
  
    Serial.print(msgString);
  
    if((rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
    } else {
      for(byte i = 0; i<len; i++){
        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        Serial.print(msgString);
      }
    }
        
    Serial.println();
  }
}

void mcpSearch(){
  if(!digitalRead(CAN0_INT)) // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)


    //getting RPM from CAN-BUS. Need to requrest it first. Most likely with "714 03 22 F4 0C 55 55 55 55".
    if(rxId == 0x77E){
      sprintf(msgString, "Found ID: 0x%.3lX       DLC: %1d  Data: ", rxId, len);
      Serial.print(msgString);
       
      Serial.println();

    }   
  }
}

void printInitialDetails(){
  Serial.println("");
  Serial.println("Welcome to MQB shiftlight");
  Serial.println("Software version: 0.0.2");
  printSpacer();
}

void printSpacer(){
  Serial.println("================");
}
