#ifndef _enhanced_h
#define _enhanced_h

#include <circle/device.h>

class COpenFile : public CDevice {
public:
  COpenFile(CDevice *m_Device, bool cascade = false) {
    m_Device = m_Device;
    m_CascadeDelete = cascade;
  }

  virtual ~COpenFile() {
    if (m_CascadeDelete) {
      delete m_Device;
      m_Device = nullptr;
    }
  }

  virtual void Close() {}

  int Read(void *pBuffer, size_t nCount) {
    return m_Device->Read(pBuffer, nCount);
  }

  int Write(void *pBuffer, size_t nCount) {
    return m_Device->Write(pBuffer, nCount);
  }

  u64 Seek(u64 offset) { return m_Device->Seek(offset); }

  u64 GetSize() { return m_Device->GetSize(); }

  int IOCtl(unsigned long ulCmd, void *pData) {
    return m_Device->IOCtl(ulCmd, pData);
  }

  boolean RemoveDevice() { return m_Device->RemoveDevice(); }

  TRegistrationHandle RegisterRemovedHandler(TDeviceRemovedHandler *pHandler,
                                             void *pContext = 0) {
    return m_Device->RegisterRemovedHandler(pHandler, pContext);
  }

  void UnregisterRemovedHandler(TRegistrationHandle hRegistration) {
    m_Device->UnregisterRemovedHandler(hRegistration);
  }

  const char *GetProperty(TProperty Property) const {
    return m_Device->GetProperty(Property);
  }

private:
  CDevice *m_Device;
  bool m_CascadeDelete;
};
#endif
