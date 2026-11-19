#ifndef _hcireader_h
#define _hcireader_h

#include "proxydevice.h"
#include <circle/input/console.h>
#include <circle/usb/usbhcidevice.h>

class CHCIReader : public CProxyDevice {
public:
  CHCIReader(CDevice *d, CUSBHCIDevice *usb) : CProxyDevice(d) {
    m_USBHCI = usb;
  }

  virtual int Read(void *pBuffer, size_t nCount);

private:
  CUSBHCIDevice *m_USBHCI;
};

#endif
