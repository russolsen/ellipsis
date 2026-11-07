//
// fatfiledevice.h
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
#ifndef _circle_nulldevice_h
#define _circle_nulldevice_h

#include "io.h"
#include <SDCard/emmc.h>
#include <circle/device.h>
#include <fatfs/ff.h>

class CFatFileDevice : public CExtendedDevice {
public:
  CFatFileDevice(void);

  ~CFatFileDevice(void);

  boolean Initialize(const char *path, char mode);

  // returns always 0
  int Read(void *pBuffer, size_t nCount);

  // returns number of written bytes
  int Write(void *pBuffer, size_t nCount);

  int GetChar();
  boolean Close();

private:
  boolean m_IsOpen;
  FIL m_File;
};

#endif
