//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2018  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "stringdevice.h"

CStringDevice::CStringDevice(void) { Close(); }

CStringDevice::~CStringDevice(void) { Close(); }

boolean CStringDevice::Close(void) {
  m_ReadPos = 0;
  m_WritePos = 0;
  return true;
}

boolean CStringDevice::Initialize(void *pBuffer, size_t nCount) {
  m_ReadPos = 0;
  m_WritePos = 0;
  Write(pBuffer, nCount);
  return true;
}

int CStringDevice::Read(void *pBuffer, size_t nCount) {
  char *pTarget = (char *)pBuffer;
  for (size_t nRead = 0; nRead < nCount; nRead++) {
    if (m_ReadPos >= m_WritePos) {
      return nRead;
    }
    pTarget[nRead++] = m_Buffer[m_ReadPos];
  }
  return (int)nCount;
}

int CStringDevice::Write(void *pBuffer, size_t nCount) {

  char *pSource = (char *)pBuffer;
  for (size_t written = 0; written < nCount; written++) {
    if (m_WritePos >= BufferSize) {
      return (int)written;
    }
    m_Buffer[m_WritePos++] = (char)pSource[written];
  }
  return (int)nCount;
}

void CStringDevice::Seek(size_t pos) { m_ReadPos = pos; }
