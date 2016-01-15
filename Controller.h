#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_
#include <SD.h>
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins


class Controller {
public:
  Controller(const char *name);
  ~Controller(){
  };
  void run();
  void printAll();
  void updateLog(File *dataFile);
  int getSet(const char *name, int value, boolean set);
  int readParamsFromEEPROM(int addr);
  int writeParamsToEEPROM(int addr);
  
private:
  const char *name;

  int measuredTemperatureInput;
  int measuredTemperatureOutput;
  int measuredTemperatureRoom;

  int minTemperatureInput;
  int maxTemperatureInput;
  int roomTemperature;
  unsigned long adjustmentLoopTimeMs;
  unsigned int pwmAdjustmentValueRoom;
  unsigned int pwmAdjustmentValueHeating;

  unsigned char pwmMin;
  unsigned char pwmMax;
  unsigned char pwmInit;
  
  unsigned int pwmOut; // divide this with 10 before using
  
private:
  long unsigned adjustentTick;
  friend class eepromParams; 
};


#endif

