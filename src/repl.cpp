#include "kernel.h"
#include <circle/alloc.h>

#include "ellipsis.h"
#include "fatfiledevice.h"
#include "prompter.h"
#include "hcireader.h"
#include "lisp.h"
#include "parser.h"

const char *STARTUP = "startup.lsp";

int repl(void) {
  CKernel *pKernel = GetKernelP();
  CConsole *console = pKernel->GetConsoleP();
  CHCIReader *hci_reader = new CHCIReader(console, pKernel->GetUSBHCIP());
  CPrintFer printfer(pKernel->GetConsoleP());
  CPrompter prompter("Ellipsis> ", hci_reader);
  CParser parser(&prompter);
  CLisp lisp(32000, &printfer, pKernel->GetLoggerP());

#if 1
  if (CFatFileDevice::Exists(STARTUP)) {
    lisp.Load(STARTUP);
  }
#endif

  while (1) {
    CCell *v = parser.ReadValue(&lisp);
    if (v == lisp.Eof()) {
      break;
    }
    CCell *result = lisp.Eval(v, 0);
    printfer.PrintF("%v\n", result);
    lisp.GC();
  }

  return 1;
}

