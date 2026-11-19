#include "prompter.h"

int CPrompter::GetChar(void) {
  if (m_LastCharRead == '\n') {
    Write(m_Prompt, m_PromptLength);
  }
  int ch = CProxyDevice::GetChar();
  m_LastCharRead = ch;
  return ch;
}




