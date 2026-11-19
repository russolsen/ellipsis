#ifndef _buffered_reader_h
#define _buffered_reader_h

#include "proxydevice.h"

class CBufferedReader : public CProxyDevice {
public:
  CBufferedReader(CDevice *d) : CProxyDevice(d) {}
  int GetChar(void);

private:
  char m_Buffer[128];
  int m_NChars;
  int m_NextChar;
};

#endif
