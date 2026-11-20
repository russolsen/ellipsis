#ifndef __repl_h_
#define __repl_h_

#include "kernel.h"
#include <circle/sched/task.h>

#include "ellipsis.h"
#include "lisp.h"
#include "parser.h"
#include "prompter.h"

class CRepl : public CTask {
public:
  void Run(void);
};
#endif
