#include "kernel.h"
#include <circle/alloc.h>

#include "ellipsis.h"
#include "enhancedconsole.h"
#include "fatfiledevice.h"
#include "io.h"
#include "lisp.h"
#include "parser.h"

const char *STARTUP = "startup.lsp";

int repl(void) {
  CKernel *pKernel = GetKernelP();

  CLisp lisp(8000, pKernel, pKernel->GetLoggerP());

  CCharPrompter prompter("Ellipsis> ", pKernel, pKernel);
  CParser parser(&prompter);

  pKernel->Print("\n\nWelcome to Ellipsis!\n\n");

  if (CFatFileDevice::Exists(STARTUP)) {
    lisp.Load(STARTUP);
  }

  while (1) {
    CCell *v = parser.ReadValue(&lisp);
    if (v == lisp.Eof()) {
      break;
    }
    CCell *result = lisp.Eval(v, 0);
    lisp.Print(result);
    pKernel->Print("\n");
  }

  return 1;
}
