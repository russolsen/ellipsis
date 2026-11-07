#include "manager.h"

CMemoryManager::CMemoryManager(const long size) {

  m_MemorySize = size;
  m_Cells = new CCons[size];
  m_PFree = 0;

  for (int i = size - 1; i >= 0; i--) {
    Release(m_Cells + i);
  }
}

void CMemoryManager::Mark(CCell *p) {
  if (!p) {
    return;
  }

  p->SetMarked(1);

  if (!IsCons(p)) {
    return;
  }

  CCons *cons = (CCons *)p;

  Mark(cons->First());

  for (cons = cons->Rest(); NotNil(cons); cons = cons->Rest()) {
    cons->SetMarked(1);
  }
}

void CMemoryManager::Sweep(void) {
  for (int i = 0; i < m_MemorySize; i++) {
    CCell *p = m_Cells + i;
    if (p->Freed()) {
      continue;
    }

    if (p->Marked()) {
      p->SetMarked(0);
      continue;
    }
    Release(p);
  }
}
