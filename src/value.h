#ifndef _value_h_
#define _value_h_

#include "io.h"
#include <circle/util.h>

extern const char *BAD_STRING;

enum ValueType : unsigned char {
  LISP_NIL,
  LISP_ERROR,
  LISP_CONS,
  LISP_INTEGER,
  LISP_SYMBOL,
  LISP_SHORT_SYMBOL,
  LISP_STRING,
  LISP_SHORT_STRING,
  LISP_PRIMITIVE,
  LISP_FUNCTION,
  LISP_MACRO,
  LISP_ENV,
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

  unsigned char Freed(void) { return m_Freed; }

  void SetFreed(unsigned char f) { m_Freed = f; }

  unsigned char Marked(void) { return m_Marked; }

  void SetMarked(unsigned char m) { m_Marked = m; }

  ValueType Type(void) { return (ValueType)m_Type; }

protected:
  unsigned char m_Type;
  unsigned char m_Flags;
  unsigned char m_Marked;
  unsigned char m_Freed;
};

class CInt : public CCell {
public:
  CInt *Morph(long v) {
    m_Type = LISP_INTEGER;
    m_Int = v;
    return this;
  }

  long GetInt(void) { return m_Int; }

private:
  long m_Int;
};

inline CInt *AsInt(CCell *v, long l) { return ((CInt *)v)->Morph(l); }

class CPrimitive : public CCell {
public:
  CPrimitive *Morph(Primitive prim) {
    m_Type = LISP_PRIMITIVE;
    m_Primitive = prim;
    return this;
  }

  Primitive GetPFunc(void) { return m_Primitive; }

private:
  Primitive m_Primitive;
};

inline CPrimitive *AsPrimitive(CCell *v, Primitive prim) {
  return ((CPrimitive *)v)->Morph(prim);
}

class CBaseString : public CCell {
public:
  CBaseString *Initialize(const char *name) {
    const unsigned int l = strlen(name) + 1;
    char *contents = new char[l];
    strcpy(contents, name);
    m_String = contents;
    return this;
  }

  const char *String(void) { return m_String; }

private:
  char *m_String;
};

class CCharString : public CBaseString {
public:
  CCharString *Morph(const char *name) {
    Initialize(name);
    m_Type = LISP_STRING;
    return this;
  }
};

inline CCharString *AsString(CCell *v, const char *name) {
  return ((CCharString *)v)->Morph(name);
}

class CSymbol : public CBaseString {
public:
  CSymbol *Morph(const char *name) {
    m_Type = LISP_SYMBOL;
    Initialize(name);
    return this;
  }
};

inline CSymbol *AsSymbol(CCell *v, const char *name) {
  return ((CSymbol *)v)->Morph(name);
}

class CCons : public CCell {
public:
  CCons(CCell *car = Nil, CCell *cdr = Nil) { Morph(car, cdr); }

  CCons *Morph(CCell *car = Nil, CCell *cdr = Nil) {
    m_Type = LISP_CONS;
    m_Car = car;
    m_Cdr = cdr;
    return this;
  }

  int Count(void);

  CCell *Car(void) { return m_Car; }
  CCell *Cdr(void) { return m_Cdr; }

  CCons *First(void) { return (CCons *)m_Car; }
  void SetFirst(CCell *p) { m_Car = p; }

  void SetRest(CCell *p) { m_Cdr = p; }
  CCons *Rest(void) { return (CCons *)m_Cdr; }

  CCell *Second(void) { return Rest()->First(); }

  CCell *Third(void) { return Rest()->Rest()->First(); }

private:
  CCell *m_Car;
  CCell *m_Cdr;
};

inline ValueType TypeOf(CCell *v) {
  if (!v) {
    return LISP_NIL;
  }
  return ((CCell *)v)->Type();
}

inline bool IsNil(CCell *v) { return TypeOf(v) == LISP_NIL; }
inline bool NotNil(CCell *v) { return TypeOf(v) != LISP_NIL; }

inline CCons *AsCons(CCell *v, CCell *car = Nil, CCell *cdr = Nil) {
  return ((CCons *)v)->Morph(car, cdr);
}

class CEnv : public CCell {
public:
  CEnv *Morph(CCons *bindings, CEnv *parent = 0) {
    m_Type = LISP_ENV;
    m_Bindings = bindings;
    m_Parent = parent;
    return this;
  }

  void SetBindings(CCons *b) { m_Bindings = b; }
  CCons *GetBindings(void) { return m_Bindings; }
  CEnv *GetParent(void) { return m_Parent; }

  bool FindBinding(CCell *sym, CCons **result) {
    for (CCons *p = m_Bindings; NotNil(p); p = p->Rest()) {
      CCons *binding = p->First();
      if (binding->Car() == sym) {
        *result = binding;
        return true;
      }
    }

    if (NotNil(m_Parent)) {
      return m_Parent->FindBinding(sym, result);
    } else {
      *result = (CCons *)Nil;
      return false;
    }
  }

private:
  CCons *m_Bindings;
  CEnv *m_Parent;
};

inline CEnv *AsEnv(CCell *v, CCons *bindings, CEnv *parent = 0) {
  return ((CEnv *)v)->Morph(bindings, parent);
}

class CFunction : public CCell {
public:
  CFunction *Morph(CCons *fdef, CEnv *env, ValueType t = LISP_FUNCTION) {
    m_Type = t;
    m_Def = fdef;
    m_Env = env;
    return this;
  }

  CCons *Def() { return m_Def; }
  CEnv *Env() { return m_Env; }

private:
  CCons *m_Def;
  CEnv *m_Env;
};

inline CFunction *AsFunction(CCell *v, CCons *fdef, CEnv *env) {
  return ((CFunction *)v)->Morph(fdef, env);
}

inline CFunction *AsMacro(CCell *v, CCons *fdef, CEnv *env) {
  return ((CFunction *)v)->Morph(fdef, env, LISP_MACRO);
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
