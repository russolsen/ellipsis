#ifndef _manager_h_
#define _manager_h_

#include "ellipsis.h"
#include "value.h"

class CMemoryManager {
public:
  CMemoryManager(const long size);

  inline CCell *Allocate(void) {
    CCons *result = m_PFree;
    m_PFree = m_PFree->Rest();
    result->Allocate();
    m_FreeCount--;
    return result;
  }

  inline void Release(CCell *p) {
    CCons *link = (CCons *)p;
    link->Release();
    link->Morph();
    link->SetRest(m_PFree);
    m_PFree = link;
  }

  long FreeCount() { return m_FreeCount; }
  long MemorySize() { return m_MemorySize; }

  void Mark(CCell *p);
  void Sweep(void);

private:
  long m_MemorySize;
  long m_FreeCount;
  CCons *m_Cells;
  CCons *m_PFree;
};

#endif
