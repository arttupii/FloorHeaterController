#ifndef _SERIALSTREAM_H_
#define _SERIALSTREAM_H_

class SerialStream{
  public:
  SerialStream();
  ~SerialStream();
  const char* getParam(int p);
  char available();
  void clear();
  private:
  unsigned char buf[50];
  int index;
};

#endif
