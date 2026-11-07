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

#include "enhancedconsole.h"
#include "ellipsis.h"

CEnhancedConsole::CEnhancedConsole(CUSBHCIDevice *pUSBHCI, CConsole *pConsole)
    : m_pUSBHCI(pUSBHCI), m_pConsole(pConsole) {}

int CEnhancedConsole::Read(void *pBuffer, size_t nCount) {
  int nResult;
  while ((nResult = m_pConsole->Read(pBuffer, nCount)) <= 0) {
    boolean bUpdated = m_pUSBHCI->UpdatePlugAndPlay();

    if (bUpdated) {
      m_pConsole->UpdatePlugAndPlay();
    }
    GetKernelP()->DelayMS(1000);
  }
  return nResult;
}

int CEnhancedConsole::Write(void *pBuffer, size_t nCount) {
  return m_pConsole->Write(pBuffer, nCount);
}
