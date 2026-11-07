//
// stringdevice.h
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
#ifndef _stringdevice_h
#define _stringdevice_h

#include <circle/device.h>

class CStringDevice : public CDevice {
public:
  static const int BufferSize = 256;

  CStringDevice(void);

  ~CStringDevice(void);

  boolean Initialize(void *, size_t);

  int Read(void *pBuffer, size_t nCount);

  int Write(void *pBuffer, size_t nCount);

  void Seek(size_t pos);

  boolean Close(void);

  inline char *GetZString(void) {
    m_Buffer[m_WritePos] = 0;
    return m_Buffer;
  }

private:
  // Leave room for a zero byte.
  char m_Buffer[BufferSize + 1];
  int m_WritePos;
  int m_ReadPos;
};

#endif
