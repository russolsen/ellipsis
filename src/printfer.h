#ifndef _printfer_h
#define _printfer_h

#include <circle/device.h>

class CPrintFer {
  public:
    CPrintFer(CDevice *d) {
      m_Device = d;
    }

    void PrintF(const char* format, ...);

    CDevice *GetDevice() { return m_Device; }

  private:
    CDevice *m_Device;
};

#endif
