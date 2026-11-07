#include "enhanceddevice.h"

#if 0
CCharPrompter::CCharPrompter(const char *prompt, CPrinter *printer,
                             CCharReader *reader) {
  m_Prompt = prompt;
  m_Printer = printer;
  m_PReader = reader;
  m_LastCharRead = '\n';
  
}

int CCharPrompter::GetChar(void) {
  if (m_LastCharRead == '\n') {
    m_Printer->Print("%s", m_Prompt);
  }
  int ch = CExtendedDevice::GetChar();
  m_LastCharRead = ch;
  return ch;
}

CBufferedCharReader::CBufferedCharReader(CDevice *d) {
  m_Device = d;
  m_NChars = -1;
  m_NextChar = 0;
}

int CExtendedDevice::GetChar(void) {
  if (m_NextChar >= m_NChars) {
    int n = m_Device->Read(m_Buffer, sizeof(m_Buffer));
    if (n <= 0) {
      return -1;
    }
    m_NChars = n;
    m_NextChar = 0;
  }
  return m_Buffer[m_NextChar++];
}
#endif
