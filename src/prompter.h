#ifndef _charprompter_h_
#define _charprompter_h_

#include "charreader.h"
#include "proxydevice.h"
#include <circle/util.h>
#include <circle/input/console.h>
#include <circle/usb/usbhcidevice.h>

class CPrompter : public CProxyDevice {
	public:
	CPrompter(const char* prompt, CDevice *d)
	: CProxyDevice(d) {
		m_Prompt = prompt;
		m_PromptLength = strlen(prompt);
		m_LastCharRead = '\n';
	}

	int GetChar(void); 

	private:

	const char* m_Prompt;
	int m_PromptLength;
	int m_LastCharRead;
};

#endif
