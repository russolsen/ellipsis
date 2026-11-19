#ifndef _proxy_device_h
#define _proxy_device_h

#include <circle/device.h>
#include "closeable.h"
#include "charreader.h"

class CProxyDevice : public CDevice, public CCloseable, public CCharReader
{
public:
	CProxyDevice(CDevice *d) {
		m_Device = d;
   	}

	virtual int Read (void *pBuffer, size_t nCount) {
		return m_Device->Read(pBuffer, nCount);
        }

	virtual int Write (const void *pBuffer, size_t nCount) {
		return m_Device->Write(pBuffer, nCount);
	}

	virtual u64 Seek (u64 ullOffset) {
		return m_Device->Seek(ullOffset);
	}

	virtual u64 GetSize (void) const {
		return m_Device->GetSize();
	}

	virtual int IOCtl (unsigned long ulCmd, void *pData) {
		return m_Device->IOCtl(ulCmd, pData);
	}

	virtual boolean RemoveDevice (void) {
		return m_Device->RemoveDevice();
	}

	virtual bool Close() {
		return true;
	}

	virtual int GetChar() {
		char ch;
		if ( m_Device->Read(&ch, 1) <= 0) {
			return -1;
		}
		return ch;
        }

	CDevice *GetRealDevice() { return m_Device; }

	private:

	CDevice *m_Device;
};

#endif
