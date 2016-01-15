#include "config.h"
#include "Controller.h"
#include <SD.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include "SerialStream.h"

Controller controllers[] = {Controller("Controller1"),Controller("Controller2")};

boolean sdCardInitialized=false;
// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int chipSelect = 4;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print(F("Initializing SD card..."));
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  int addr = 0;
  for(int i=0;i<controllersCnt();i++) {
    addr = controllers[i].readParamsFromEEPROM(addr);
  }
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println(F("Card failed, or not present"));
    // don't do anything more:
    sdCardInitialized = true;
    return;
  }
  Serial.println(F("card initialized."));
}

int controllersCnt(){
    return sizeof(controllers)/sizeof(Controller);
}

SerialStream serialStream;

void serialMenu() {
  if(serialStream.available()) {
    if(strcmp_P(serialStream.getParam(0), PSTR("HELP"))==0) {
      Serial.println(F("LOG - dumps log file"));
      Serial.println(F("ALL - Get all"));
      Serial.println(F("SET X Y Z - Set param"));
      Serial.println(F("GET X Y - get param"));
      Serial.println(F("SAVE - save params to EEPROM"));
      Serial.println(F("RESTORE - read params from EEPROM"));
    } 
    else if(strcmp_P(serialStream.getParam(0), PSTR("SAVE"))==0) {
      Serial.print("OK:");
      int addr = 0;
      for(int i=0;i<controllersCnt();i++) {
        addr = controllers[i].writeParamsToEEPROM(addr);
      }
    } else
    if(strcmp_P(serialStream.getParam(0), PSTR("RESTORE"))==0) {
      Serial.print("OK:\n");
      int addr = 0;
      for(int i=0;i<controllersCnt();i++) {
        addr = controllers[i].readParamsFromEEPROM(addr);
      }
    } 
    else if(strcmp_P( serialStream.getParam(0), PSTR("ALL"))==0) {
      Serial.print("OK:\n[");
      for(int i=0;i<controllersCnt();i++) {
        controllers[i].printAll();
        if(i<controllersCnt()-1) Serial.print(",\n");
      }
      Serial.println("]");     
    } 
    else if(strcmp_P( serialStream.getParam(0), PSTR("SET"))==0 || strcmp_P( serialStream.getParam(0), PSTR("GET"))==0) {
      int c = atoi(serialStream.getParam(1));
      if(c>=controllersCnt() || c<0) {
        Serial.println(F("ERROR: Invalid controlId"));
      } else {
        controllers[c].getSet(serialStream.getParam(2), atoi(serialStream.getParam(3)), strcmp_P( serialStream.getParam(0), PSTR("SET"))==0);
      }
    } else {
      Serial.println(F("ERROR: Invalid cmd"));
    }
    serialStream.clear();
  }
}

void loop(){
  for(int i=0;i<controllersCnt();i++) {
    controllers[i].run();
  }


  serialMenu();

  //Update loggin data
  /*  if(sdCardInitialized) {
   static unsigned long loggerTick = millis();
   if(millis()+60000>loggerTick) {
   loggerTick = millis() + 60000;
   File dataFile = SD.open("datalog.txt", FILE_WRITE);
   if (dataFile) {
   controller1.updateLog(&dataFile);
   controller2.updateLog(&dataFile);
   dataFile.close();
   }  
   // if the file isn't open, pop up an error:
   else {
   Serial.println("error opening datalog.txt");
   } 
   }
   }*/
}






