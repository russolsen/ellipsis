//
// kernel.h
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2017  R. Stange <rsta2@o2online.de>
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
#ifndef _kernel_h
#define _kernel_h

#include <SDCard/emmc.h>
#include <circle/actled.h>
#include <circle/devicenameservice.h>
#include <circle/exceptionhandler.h>
#include <circle/input/console.h>
#include <circle/interrupt.h>
#include <circle/koptions.h>
#include <circle/logger.h>
#include <circle/screen.h>
#include <circle/serial.h>
#include <circle/timer.h>
#include <circle/types.h>
#include <circle/usb/usbhcidevice.h>
#include <fatfs/ff.h>

#include "enhancedconsole.h"
#include "io.h"

enum TShutdownMode { ShutdownNone, ShutdownHalt, ShutdownReboot };

class CKernel : public CCharReader, public CPrinter {
public:
  CKernel(void);
  ~CKernel(void);

  boolean Initialize(void);

  TShutdownMode Run(void);
  int GetChar(void);
  int PutChar(int);
  void ReadLine(void);
  void Print(const char *, ...);
  void DelayMS(const unsigned);

  inline CEnhancedConsole *GetEConsoleP(void) { return &m_EConsole; }

  inline CConsole *GetConsoleP(void) { return &m_Console; }

  inline CLogger *GetLoggerP(void) { return &m_Logger; }

private:
  // do not change this order
  CActLED m_ActLED;
  CKernelOptions m_Options;
  CDeviceNameService m_DeviceNameService;
  CScreenDevice m_Screen;
  CSerialDevice m_Serial;
  CExceptionHandler m_ExceptionHandler;
  CInterruptSystem m_Interrupt;
  CTimer m_Timer;
  CLogger m_Logger;

  CUSBHCIDevice m_USBHCI;

  CConsole m_Console;
  CEnhancedConsole m_EConsole;

  CEMMCDevice m_EMMC;
  FATFS m_FileSystem;

  char m_LineBuffer[501];
};

void Print(const char *, ...);

#endif
