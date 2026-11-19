#ifndef _value_h_
#define _value_h_

#include <circle/util.h>
#include "printfer.h"

extern const char *BAD_STRING;

enum ValueType : unsigned char {
  LISP_NIL,
  LISP_ERROR,
  LISP_CONS,
  LISP_INTEGER,
  LISP_SYMBOL,
  LISP_STRING,
  LISP_PRIMITIVE,
  LISP_FUNCTION,
  LISP_MACRO,
  LISP_ENV,
  LISP_SHORT_STRING,
  LISP_SHORT_SYMBOL,
  LISP_EMPTY = 0xcc,
  LISP_RAWCELL = 0xfe,
  LISP_INVALID = 0xff,
};

class CCell;
class CCons;
class CEnv;
class CLisp;

extern CCell *Nil;

typedef CCell *(*Primitive)(CLisp *, CCons *, CEnv *);

class CCell {
public:
  void Allocate(void) {
    SetFreed(0);
    SetMarked(0);
  }

  void Release(void) {
    SetFreed(1);
    SetMarked(0);
  }

  virtual ValueType Type() { return LISP_RAWCELL; }
  virtual CCell *Car(void) { return Nil; }
  virtual CCell *Cdr(void) { return Nil; }

  virtual void Print(CPrintFer *p) { 
    p->PrintF("Cell");
  }

  unsigned char Freed(void) { return m_Freed; }

  void SetFreed(unsigned char f) { m_Freed = f; }

  unsigned char Marked(void) { return m_Marked; }

  void SetMarked(unsigned char m) { m_Marked = m; }

protected:
  unsigned char m_Flags;
  unsigned char m_Marked;
  unsigned char m_Freed;
};

class CCons : public CCell {
public:
  CCons(CCell *car = Nil, CCell *cdr = Nil) {
    m_Car = car;
    m_Cdr = cdr;
  }

  void* operator new(size_t st, CCons* p ) { return p; }

  virtual ValueType Type() { return LISP_CONS; }

  int Count();

  CCell *Car(void) { return m_Car; }
  CCell *Cdr(void) { return m_Cdr; }

  CCons *First(void) { return (CCons *)m_Car; }
  void SetFirst(CCell *p) { m_Car = p; }

  void SetRest(CCell *p) { m_Cdr = p; }
  CCons *Rest(void) { return (CCons *)m_Cdr; }

  CCell *Second(void) { return Rest()->First(); }

  CCell *Third(void) { return Rest()->Rest()->First(); }

  CCell *Last() {
    CCell *result = Nil;

    for(CCons *p = this; p; p = p->Rest()) {
	result = p->First();
    }
    return result;
  }

private:
  CCell *m_Car;
  CCell *m_Cdr;
};

inline CCons *AsCons(CCell *v, CCell *car = 0, CCell *cdr = 0) {
  CCons *c = new((CCons *)v) CCons(car, cdr);
  return c;
}

class CInt : public CCell {
public:
  CInt(long v) {
    m_Int = v;
  }

  CInt() {
    m_Int = 0;
  }

  void* operator new(size_t st, CInt* p ) { return p; }

  ValueType Type() { return LISP_INTEGER; }

  void Print(CPrintFer *p) { p->PrintF("%d", m_Int); }

  long GetInt(void) { return m_Int; }

private:
  long m_Int;
};

inline CInt *AsInt(CCell *v, long l)
{
  CInt *c = new((CInt *)v) CInt(l);
  return c;
}

class CPrimitive : public CCell {
public:
  CPrimitive(Primitive prim) {
    m_Primitive = prim;
  }

  void* operator new(size_t st, CPrimitive* p ) { return p; }

  ValueType Type() { return LISP_PRIMITIVE; }

  Primitive GetPFunc(void) { return m_Primitive; }

private:
  Primitive m_Primitive;
};

inline CPrimitive *AsPrimitive(CCell *v, Primitive prim) {
  CPrimitive *p = new((CPrimitive *)v) CPrimitive(prim);
  return p;
}

class CBaseString : public CCell {
public:
  CBaseString(const char *name) {
    const unsigned int l = strlen(name) + 1;
    char *contents = new char[l];
    strcpy(contents, name);
    m_String = contents;
  }

  const char *String(void) { return m_String; }

private:
  char *m_String;
};

class CCharString : public CBaseString {
  public:

  CCharString(const char *name) : CBaseString(name) {}

  void* operator new( size_t, CCharString* p ) { return p; }

  ValueType Type() { return LISP_STRING; }

  void Print(CPrintFer *p) { p->PrintF("\"%s\"", String()); }
};

inline CCharString *AsString(CCell *v, const char *name) {
  CCharString *s = new((CCharString *)v) CCharString(name);
  return s;
}

class CSymbol : public CBaseString {
public:
  void* operator new( size_t, CSymbol* p ) { return p; }

  CSymbol(const char *name) : CBaseString(name) {}

  ValueType Type() { return LISP_SYMBOL; }

  void Print(CPrintFer *p) { p->PrintF("%s", String()); }
};

inline CSymbol *AsSymbol(CCell *v, const char *name) {
  CSymbol *s = new((CSymbol *)v) CSymbol(name);
  return s;
}

inline ValueType TypeOf(CCell *v) {
  if (!v) {
    return LISP_NIL;
  }
  return ((CCell *)v)->Type();
}

inline bool IsNil(CCell *v) { return TypeOf(v) == LISP_NIL; }
inline bool NotNil(CCell *v) { return TypeOf(v) != LISP_NIL; }

class CEnv : public CCell {
public:
  CEnv(CCons *bindings, CEnv *parent = 0) {
    m_Bindings = bindings;
    m_Parent = parent;
  }

  void* operator new( size_t, CEnv* p ) { return p; }

  ValueType Type() { return LISP_ENV; };

  void SetBindings(CCons *b) { m_Bindings = b; }
  CCons *GetBindings(void) { return m_Bindings; }
  CEnv *GetParent(void) { return m_Parent; }

  bool FindBinding(CCell *sym, CCons **result=nullptr) {
    for (CCons *p = m_Bindings; NotNil(p); p = p->Rest()) {
      CCons *binding = p->First();
      if (binding->Car() == sym) {
	if (result) *result = binding;
        return true;
      }
    }

    if (NotNil(m_Parent)) {
      return m_Parent->FindBinding(sym, result);
    } else {
      if (result) *result = (CCons *)Nil;
      return false;
    }
  }

private:
  CCons *m_Bindings;
  CEnv *m_Parent;
};

#if 1
inline CEnv *AsEnv(CCell *v, CCons *bindings, CEnv *parent = 0) {
  CEnv *e = new((CEnv *)v) CEnv(bindings, parent);
  //CEnv *e = new(v) CEnv(bindings, parent);
  return e;
}
#endif


class CFunction : public CCell {
public:
  CFunction(CCons *fdef, CEnv *env) {
    m_Def = fdef;
    m_Env = env;
  }

  void* operator new( size_t, CFunction* p ) { return p; }

  ValueType Type() { return LISP_FUNCTION; };

  CCons *Def() { return m_Def; }
  CEnv *Env() { return m_Env; }

private:
  CCons *m_Def;
  CEnv *m_Env;
};

inline CFunction *AsFunction(CCell *v, CCons *fdef, CEnv *env) {
  CFunction *f = new((CFunction *)v) CFunction(fdef, env);
  return f;
}

class CMacro : public CFunction {
  public:
    CMacro(CCons *fdef, CEnv *env) : CFunction(fdef, env) {}

    void *operator new( size_t, CMacro *p) { return p; }

    ValueType Type() { return LISP_MACRO; };
};

inline CMacro *AsMacro(CCell *v, CCons *fdef, CEnv *env) {
  CMacro *f = new((CMacro *)v) CMacro(fdef, env);
  return f;
}

inline bool IsTruthy(CCell *v) { return v != Nil; }

inline bool IsInt(CCell *v) { return TypeOf(v) == LISP_INTEGER; }

inline bool IsCons(CCell *v) { return TypeOf(v) == LISP_CONS; }

inline bool IsCell(CCell *v) { return !IsInt(v); }

inline bool IsProperList(CCell *v) {
  if (!IsCons(v))
    return false;

  CCons *l = (CCons *)v;

  if (TypeOf(l->Cdr()) == LISP_CONS)
    return true;
  else if (IsNil(l->Cdr()))
    return true;
  return false;
}

#endif
