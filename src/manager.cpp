#include "manager.h"

CMemoryManager::CMemoryManager(const long size) {

  m_MemorySize = size;
  m_FreeCount = size;
  m_Cells = new CCons[size];
  m_PFree = 0;

  for (int i = size - 1; i >= 0; i--) {
    Release(m_Cells + i);
  }
}

void CMemoryManager::Mark(CCell *v) {
  if (!v)
    return;

  if (v->Marked())
    return;

  ValueType t = TypeOf(v);

  if (t == LISP_CONS) {
    CCons *c = (CCons *)v;
    while (IsCons(c)) {
      c->SetMarked(1);
      Mark(c->First());
      c = c->Rest();
    }

    if (c) {
      Mark(c);
    }
  }
  else if (t == LISP_FUNCTION || t == LISP_MACRO) {
    v->SetMarked(1);
    CFunction *f = (CFunction *)v;
    Mark(f->Def());
    Mark(f->Env());
  }
  else if (t == LISP_ENV) {
    v->SetMarked(1);
    CEnv *e = (CEnv *)v;
    Mark(e->GetBindings());
    Mark(e->GetParent());
  }
  else {
    v->SetMarked(1);
  }
}

void CMemoryManager::Sweep(void) {
  m_FreeCount = 0;

  for (int i = 0; i < m_MemorySize; i++) {
    CCell *p = m_Cells + i;
    if (p->Freed()) {
      m_FreeCount++;
      continue;
    }

    if (p->Marked()) {
      p->SetMarked(0);
      continue;
    }
    m_FreeCount++;
    Release(p);
  }
}
