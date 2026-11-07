#ifndef _io_h
#define _io_h

#include <circle/device.h>

class CPrinter {
public:
  virtual void Print(const char *, ...) = 0;
};

class CCharReader {
public:
  virtual int GetChar() = 0;
};

class CExtendedDevice : public CDevice, public CCharReader {
public:
  virtual int GetChar(void) = 0;
  virtual bool Close(void) = 0;
};

class CCharPrompter : public CCharReader {
public:
  CCharPrompter(const char *, CPrinter *, CCharReader *);
  int GetChar(void);
  bool Close(void) { return true; }

private:
  CCharReader *m_PReader;
  CPrinter *m_Printer;
  const char *m_Prompt;
  int m_LastCharRead;
};

class CBufferedCharReader : public CCharReader {
public:
  CBufferedCharReader(CDevice *d);

  int GetChar(void);
  CDevice *GetDevice(void) { return m_Device; }

private:
  CDevice *m_Device;
  char m_Buffer[128];
  int m_NChars;
  int m_NextChar;
};

#endif
