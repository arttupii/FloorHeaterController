#include "SerialStream.h"
#include "Arduino.h"
SerialStream::SerialStream()
{
  index = 0;
  buf[0] =0 ; 
}
SerialStream::~SerialStream(){
}
void SerialStream::clear() {
  index = 0;
}
const char* SerialStream::getParam(int p) {
  static char b[50];
  memcpy(b,this->buf, sizeof(b));
  b[sizeof(b)-1] = 0;
  const char *ret = 0;
  for(int i=0;i<sizeof(b);i++){
    if(b[i]==0) return ret;
    if(p==0) {
      ret = &b[i];
      p--;
    }
    if(b[i]==' ' || b[i]=='\n') {
      p--;
      b[i] = 0;
    }
  }
  return ret;
}

char SerialStream::available() {
  if(Serial.available()) {
    int incomingByte = Serial.read();
    if(incomingByte!='\n'||incomingByte!='\r') {
      buf[index] = incomingByte;
      index++;
      if(index>=sizeof(buf)) index=sizeof(buf)-1;
    }
    buf[index] = 0;

    if(incomingByte=='\n') {
      buf[index] = 0;
      index = 0;
      return true;
    }
    return false;
  }
}

