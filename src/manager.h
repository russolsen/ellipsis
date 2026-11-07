#ifndef _manager_h_
#define _manager_h_

#include "ellipsis.h"
#include "value.h"

class CMemoryManager {
public:
  const int Size = 16000;

  CMemoryManager(const long size);

  inline CCell *Allocate(void) {
    CCons *result = m_PFree;
    m_PFree = m_PFree->Rest();
    result->Allocate();
    return result;
  }

  inline void Release(CCell *p) {
    CCons *link = (CCons *)p;
    link->Release();
    link->Morph();
    link->SetRest(m_PFree);
    m_PFree = link;
  }

  void Mark(CCell *p);
  void Sweep(void);

private:
  long m_MemorySize;
  CCons *m_Cells;
  CCons *m_PFree;
};

#endif
