#include "fatfiledevice.h"

CFatFileDevice::CFatFileDevice(void) { m_IsOpen = false; }

CFatFileDevice::~CFatFileDevice(void) {
  if (m_IsOpen) {
    Close();
  }
}

boolean CFatFileDevice::Initialize(const char *path, char mode) {
  FRESULT Result = f_open(&m_File, path, mode);
  m_IsOpen = (Result == FR_OK);
  return m_IsOpen;
}

int CFatFileDevice::GetChar() {
  int ch = 0;
  int n = Read(&ch, 1);
  if (n <= 0) {
    return -1;
  }
  return ch;
}

boolean CFatFileDevice::Close(void) {
  if (m_IsOpen) {
    f_close(&m_File);
    m_IsOpen = false;
    return true;
  } else {
    return false;
  }
}

int CFatFileDevice::Read(void *pBuffer, size_t nCount) {
  size_t nRead;
  FRESULT Result = f_read(&m_File, pBuffer, nCount, &nRead);

  if ((Result == FR_OK) && (nRead > 0)) {
    return nRead;
  }
  return -1;
}

int CFatFileDevice::Write(void *pBuffer, size_t nCount) {
  unsigned nBytesWritten;

  FRESULT Result =
      f_write(&m_File, (const char *)pBuffer, nCount, &nBytesWritten);
  if (Result == FR_OK) {
    return nBytesWritten;
  } else {
    return -1;
  }
}
