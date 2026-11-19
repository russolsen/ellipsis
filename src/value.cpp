#include "value.h"
#include "kernel.h"

CCell *Nil = 0;

const char *BAD_STRING = "*Bad String*";

int CCons::Count(void) {
  int n = 0;
  for (CCons *p = this; p; p = p->Rest()) {
    n++;
  }
  return n;
}

void CCons::Print(CPrintFer *pr) {
  if (!IsProperList(this)) {
    pr->PrintF("%v.%v", Car(), Cdr());
    return;
  }

  pr->PrintF("(");
  for(CCons *this_one = this; this_one; this_one = this_one->Rest()) {
    CCell *first = this_one->Car();
    CCell *rest = this_one->Cdr();
    pr->PrintF("%v", first);
    if (rest) {
      if(IsCons(rest)) pr->PrintF(" ");
      else pr->PrintF(".%v", rest);
    }
  }
  pr->PrintF(")");
}

void CEnv::Print(CPrintFer *pr) {
  pr->PrintF("%v", GetBindings());
}

void CPrimitive::Print(CPrintFer *pr) {
  pr->PrintF("#Prim(%x)", (int)m_Primitive);
}

void CFunction::Print(CPrintFer *pr) {
  pr->PrintF("(fn %v %v)", Def()->First(), Def()->Second());
}

void CMacro::Print(CPrintFer *pr) {
  pr->PrintF("(macro %v %v)", Def()->First(), Def()->Second());
}
