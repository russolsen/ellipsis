#include "hcireader.h"

int CHCIReader::Read(void *pBuffer, size_t nCount) {
	int nResult;
	while ((nResult = CProxyDevice::Read(pBuffer, nCount)) <= 0) {
		boolean bUpdated = m_USBHCI->UpdatePlugAndPlay();

		if (bUpdated) {
			((CConsole *)GetRealDevice())->UpdatePlugAndPlay();
		}
		//GetKernelP()->DelayMS(1000);
	}
	return nResult;
}


