#include "value.h"
#include "kernel.h"

CCell *Nil = 0;

const char *BAD_STRING = "*Bad String*";

int CCons::Count(void) {
  if (m_Type != LISP_CONS) {
    return -1;
  }

  int n = 0;
  for (CCons *p = this; p; p = p->Rest()) {
    n++;
  }
  return n;
}
