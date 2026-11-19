#include "bufferedreader.h"

int CBufferedReader::GetChar(void) {
  if (m_NextChar >= m_NChars) {
    int n = Read(m_Buffer, sizeof(m_Buffer));
    if (n <= 0) {
      return -1;
    }
    m_NChars = n;
    m_NextChar = 0;
  }
  return m_Buffer[m_NextChar++];
}
