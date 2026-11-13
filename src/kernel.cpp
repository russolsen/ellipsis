//
// kernel.cpp
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
#include "kernel.h"
#include "fatfiledevice.h"
#include <circle/string.h>

#define DRIVE "SD:"
//#define DRIVE		"USB:"
//#define DRIVE		"FD:"

#define FILENAME "/startup.lsp"

// Put in .h file TBD

extern int repl(void);

static const char FromKernel[] = "kernel";

CKernel::CKernel(void)
    : m_Screen(m_Options.GetWidth(), m_Options.GetHeight()),
      m_Serial(&m_Interrupt, TRUE), m_Timer(&m_Interrupt),
      m_Logger(m_Options.GetLogLevel(), &m_Timer),
      m_USBHCI(&m_Interrupt, &m_Timer, TRUE), m_Console(&m_Serial, TRUE),
      m_EConsole(&m_USBHCI, &m_Console),
      m_EMMC(&m_Interrupt, &m_Timer, &m_ActLED) {
  m_ActLED.Blink(5); // show we are alive
}

CKernel::~CKernel(void) {}

boolean CKernel::Initialize(void) {
  boolean bOK = TRUE;

  if (bOK) {
    bOK = m_Screen.Initialize();
  }

  if (bOK) {
    bOK = m_Serial.Initialize(115200);
  }

  if (bOK) {
    CDevice *pTarget =
        m_DeviceNameService.GetDevice(m_Options.GetLogDevice(), FALSE);
    if (pTarget == 0) {
      pTarget = &m_Screen;
    }

    bOK = m_Logger.Initialize(pTarget);
  }

  if (bOK) {
    bOK = m_Interrupt.Initialize();
  }

  if (bOK) {
    bOK = m_Timer.Initialize();
  }

  if (bOK) {
    bOK = m_USBHCI.Initialize();
  }

  if (bOK) {
    bOK = m_Console.Initialize();
  }

  if (bOK) {
    bOK = m_EMMC.Initialize();
  }

  return bOK;
}

TShutdownMode CKernel::Run(void) {
  m_Logger.Write(FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);

  // Mount file system
  if (f_mount(&m_FileSystem, DRIVE, 1) != FR_OK) {
    m_Logger.Write(FromKernel, LogPanic, "Cannot mount drive: %s", DRIVE);
  }

  // Run the repl.
  repl();

  // Unmount file system
  if (f_mount(0, DRIVE, 0) != FR_OK) {
    m_Logger.Write(FromKernel, LogPanic, "Cannot unmount drive: %s", DRIVE);
  }

  return ShutdownHalt;
}

int CKernel::GetChar(void) {
  char ch;
  int n;

  while ((n = m_Console.Read(&ch, 1)) <= 0) {
    boolean bUpdated = m_USBHCI.UpdatePlugAndPlay();
    if (bUpdated) {
      m_Console.UpdatePlugAndPlay();
    }
    m_Timer.MsDelay(100);
  }

  return ch;
}

int CKernel::PutChar(int ch) {
  m_Console.Write(&ch, 1);
  return ch;
}

void CKernel::ReadLine(void) {
  int nResult;

  while ((nResult = m_Console.Read(m_LineBuffer, sizeof m_LineBuffer - 1)) <=
         0) {
    boolean bUpdated = m_USBHCI.UpdatePlugAndPlay();

    if (bUpdated) {
      m_Console.UpdatePlugAndPlay();
    }
    m_Timer.MsDelay(1000);
  }

  assert(nResult < (int)sizeof m_LineBuffer);
  assert(m_LineBuffer[nResult - 1] == '\n');
  m_LineBuffer[nResult - 1] = '\0';
}

void CKernel::Print(const char *pFormat, ...) {
  assert(pFormat != 0);

  va_list var;
  va_start(var, pFormat);

  CString Message;
  Message.FormatV(pFormat, var);

  m_Console.Write(Message, Message.GetLength());

  va_end(var);
}

void CKernel::DelayMS(const unsigned ms) { m_Timer.MsDelay(ms); }
