#include "Controller.h"
#include "Arduino.h"
#include "print.h"
#include <EEPROM.h>

#define VERSION 1

const char name_str[] PROGMEM = "name";
const char measuredTemperatureInput_str[] PROGMEM = "measuredTemperatureInput";
const char measuredTemperatureOutput_str[] PROGMEM = "measuredTemperatureOutput";
const char measuredTemperatureRoom_str[] PROGMEM = "measuredTemperatureRoom";

const char minTemperatureInput_str[] PROGMEM = "minTemperatureInput";
const char maxTemperatureInput_str[] PROGMEM = "maxTemperatureInput";

const char roomTemperature_str[] PROGMEM = "roomTemperature";
const char adjustmentLoopTimeMs_str[] PROGMEM = "adjustmentLoopTimeMs";
const char pwmAdjustmentValueRoom_str[] PROGMEM = "pwmAdjustmentValueRoom";
const char pwmAdjustmentValueHeating_str[] PROGMEM = "pwmAdjustmentValueHeating";

const char pwmMax_str[] PROGMEM = "pwmMax";
const char pwmMin_str[] PROGMEM = "pwmMin";

const char pwmOut_str[] PROGMEM = "pwmOut";
const char pwmInit_str[] PROGMEM = "pwmInit";

Controller::Controller(const char *name){
  this->name = name;
  measuredTemperatureInput = 0;
  measuredTemperatureOutput = 0;
  measuredTemperatureRoom = 0;

  minTemperatureInput = 270;
  maxTemperatureInput = 370;

  roomTemperature = 250;

  adjustmentLoopTimeMs=4000; //Säätyy 15minuutissa ääriasennosta toiseen, jos adjustment arvo on 10;
  pwmAdjustmentValueRoom = 5;
  pwmAdjustmentValueHeating = 10;

  adjustentTick = 0;
  pwmOut = 0; // divide this with 10

  pwmMin = 0;
  pwmMax = 255;
  pwmInit = 0;
}

struct eepromParams {
  int version;
  int minTemperatureInput;
  int maxTemperatureInput;
  int roomTemperature;
  unsigned long adjustmentLoopTimeMs;
  unsigned int pwmAdjustmentValueRoom;
  unsigned int pwmAdjustmentValueHeating;

  unsigned char pwmMin;
  unsigned char pwmMax;
  unsigned char pwmInit;
  
  void read(Controller *c) {
    c->minTemperatureInput = minTemperatureInput;
    c->maxTemperatureInput = maxTemperatureInput;
    c->roomTemperature = roomTemperature;
    c->adjustmentLoopTimeMs = adjustmentLoopTimeMs;
    c->pwmAdjustmentValueRoom = pwmAdjustmentValueRoom;
    c->pwmAdjustmentValueHeating =pwmAdjustmentValueHeating;
    c->pwmMin = pwmMin;
    c->pwmMax = pwmMax;
    c->pwmInit = pwmInit;
  }
  void write(Controller *c) {
    minTemperatureInput = c->minTemperatureInput;
    maxTemperatureInput = c->maxTemperatureInput;
    roomTemperature = c->roomTemperature;
    adjustmentLoopTimeMs = c->adjustmentLoopTimeMs;
    pwmAdjustmentValueRoom = c->pwmAdjustmentValueRoom;
    pwmAdjustmentValueHeating = c->pwmAdjustmentValueHeating;
    pwmMin = c->pwmMin;
    pwmMax = c->pwmMax;
    pwmInit = c->pwmInit;
  }
};

int Controller::readParamsFromEEPROM(int addr){
  struct eepromParams tmp;
  for(int i=0;i<sizeof(tmp);i++) {
    char *b = (char*)&tmp;
    b[i] = EEPROM.read(addr); 
    addr++;
  }
  if(tmp.version==VERSION)  {
    tmp.read(this);
  } else {
     Serial.println("ERROR: Invalid VERSION"); 
  }
  return addr;
}

int Controller::writeParamsToEEPROM(int addr){
  struct eepromParams tmp;
  tmp.write(this);
  tmp.version = VERSION;
  for(int i=0;i<sizeof(tmp);i++) {
    char *b = (char*)&tmp;
    if(EEPROM.read(addr)!=b[i]) EEPROM.write(addr, b[i]); 
    addr++;
  }
  return addr;
}
  
void printParam(const int index, const __FlashStringHelper * name, const int value, boolean last=false) {
  Serial.print(" ");
  Serial.print(name);
  Serial.print(": ");
  Serial.print(value);
  if(!last) Serial.println(",");
}

void printParam(const int index, const __FlashStringHelper * name, const char* value, boolean last=false) {
  Serial.print(" ");
  Serial.print(name);
  Serial.print(": ");  
  Serial.print(value);
  if(!last) Serial.printlnALL(",");
}


void paramToFile(File*dataFile, int value) {
  dataFile->print(value);
  dataFile->print(",");
}
void paramToFile(File*dataFile, const char* value) {
  dataFile->print(value);
  dataFile->print(",");
}

void Controller::updateLog(File *dataFile) {
  paramToFile(dataFile,millis()/1000);
  paramToFile(dataFile,name);
  paramToFile(dataFile,measuredTemperatureInput);
  paramToFile(dataFile,measuredTemperatureOutput);
  paramToFile(dataFile,measuredTemperatureRoom);

  paramToFile(dataFile,minTemperatureInput);
  paramToFile(dataFile,maxTemperatureInput);

  paramToFile(dataFile,roomTemperature);
  paramToFile(dataFile,adjustmentLoopTimeMs);
  paramToFile(dataFile,pwmAdjustmentValueRoom);
  paramToFile(dataFile,pwmAdjustmentValueHeating);
  
  dataFile->println(pwmOut/10);
}

int Controller::getSet(const char *name, int value, boolean set){
   #define SETGET(PARAM) \
   if(strcmp_P(name, PARAM##_str)==0) { \
     if(set) { \
       serialPrintf(F("OK: Set new value: Controller:%s, param:%s, value:%d\n"), this->name, name, value);\
       this->PARAM = value; \
     } else { \
       serialPrintf(F("OK:\n"));\
       serialPrintf(F("{"));\
       serialPrintfP(PARAM##_str);\
       serialPrintf(F(": %d}\n"), this->PARAM);\
     }\
   }
  
   SETGET(measuredTemperatureOutput)
   else SETGET(measuredTemperatureInput)
   else SETGET(measuredTemperatureOutput)
   else SETGET(measuredTemperatureRoom)
   else SETGET(minTemperatureInput)
   else SETGET(maxTemperatureInput)
   else SETGET(roomTemperature)
   else SETGET(adjustmentLoopTimeMs)
   else SETGET(pwmAdjustmentValueRoom)
   else SETGET(pwmAdjustmentValueHeating)
   else SETGET(pwmMax)
   else SETGET(pwmMin)
   else SETGET(pwmOut)
   else SETGET(pwmInit)
   else {
      Serial.println(F("ERROR: invalid param name"));
   }
}


void Controller::printAll(){
  Serial.println(F("{"));
  printParam(0,F("name"), name);
  printParam(0,F("measuredTemperatureInput"), measuredTemperatureInput);
  printParam(1,F("measuredTemperatureOutput"), measuredTemperatureOutput);
  printParam(2,F("measuredTemperatureRoom"), measuredTemperatureRoom);

  printParam(3,F("minTemperatureInput"), minTemperatureInput);
  printParam(4,F("maxTemperatureInput"), maxTemperatureInput);

  printParam(5,F("roomTemperature"), roomTemperature);
  printParam(6,F("adjustmentLoopTimeMs"), adjustmentLoopTimeMs);
  printParam(7,F("pwmAdjustmentValueRoom"), pwmAdjustmentValueRoom);
  printParam(8,F("pwmAdjustmentValueHeating"), pwmAdjustmentValueHeating);

  printParam(9,F("pwmMax"), pwmMax);
  printParam(9,F("pwmMin"), pwmMin);

  printParam(9,F("pwmOut"), pwmOut, true);
  Serial.print("}");
}

void Controller::run(){
  if(millis() + adjustmentLoopTimeMs > adjustentTick) {
    adjustentTick = millis() + adjustmentLoopTimeMs;

    if(measuredTemperatureInput<minTemperatureInput) {
      //Kasvata lämpötilaa. Sisääntuleva lämpötila on liian pieni. Lattia on liian kylmä
      pwmOut += pwmAdjustmentValueHeating;
    } 
    else if(measuredTemperatureInput>maxTemperatureInput) {
      //Pienennä lämpötilaa. Sisääntuleva lämpötila on liian suuri. Lattia on liian lämmin
      pwmOut -= pwmAdjustmentValueHeating;
    } 
    else if(measuredTemperatureRoom<roomTemperature) {
      //Kasvata lämpötilaa. Huoneessa on liian kylmä.
      pwmOut += pwmAdjustmentValueRoom;
    } 
    else if(measuredTemperatureRoom>roomTemperature) {
      //Piennnä lämpötilaa. Huoneessa on liian lämmintä.
      pwmOut -= pwmAdjustmentValueRoom;
    } 
    else {
      //Sopivalämpötila saavutettu :) 
    }

    if(pwmOut/10 > pwmMax) pwmOut = pwmMax*10;
    if(pwmOut/10 < pwmMin) pwmOut = pwmMin*10;   
  }
}




