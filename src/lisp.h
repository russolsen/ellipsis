#ifndef _lisp_h_
#define _lisp_h_

#include "utils.h"
#include "printfer.h"
#include "kernel.h"
#include "manager.h"
#include "value.h"

extern CError *Error;

class CLisp {
public:
  CLisp(long, CPrintFer *, CLogger *);

  CCell *Eval(CCell *, CEnv * = 0);

  CCons *EvalItems(CCons *, CEnv *);

  void Print(CCell *expr, const char *spacer = " ");

  CCell *Load(const char *);

  bool GC();

  void DoGC();

  CCell *Equal(CCell *, CCell *);

  CCons *Map(CCell *, CCons *, CEnv *);

  CCell *Reduce(CCell *f, CCons *lst, CEnv *env);

  CCons *MakeList2(CCell *a, CCell *b);

  CCons *MakeListZ(CCell *first_arg, ...);

  CCons *Cons(CCell *first = Nil, CCell *rest = Nil) {
    CCell *cell = m_PManager->Allocate();
    return AsCons(cell, first, rest);
  }

  const int N_CACHED_INTS = 13;
  const int CACHED_INT_BIAS = 3;
  const int MIN_CACHED_INT = 1 - CACHED_INT_BIAS;
  const int MAX_CACHED_INT = N_CACHED_INTS - CACHED_INT_BIAS;

  CInt *MakeInt(long v = 0) {
    if ((v <= MAX_CACHED_INT) && (v >= MIN_CACHED_INT)) {
      return m_Ints + (v + CACHED_INT_BIAS - 1);
    }
    CCell *cell = m_PManager->Allocate();
    return AsInt(cell, v);
  }

  CCell *MakeBool(int b) {
    if (b) {
      return m_SymTrue;
    }
    return Nil;
  }

  CSymbol *MakeSymbol(const char *name) {
    CCell *cell = m_PManager->Allocate();
    return AsSymbol(cell, name);
  }

  CCharString *MakeString(const char *name) {
    CCell *cell = m_PManager->Allocate();
    return AsString(cell, name);
  }

  CPrimitive *MakePrimitive(Primitive p) {
    CCell *cell = m_PManager->Allocate();
    return AsPrimitive(cell, p);
  }

  CEnv *MakeEnv(CCons *bindings = 0, CEnv *parent = 0) {
    CCell *cell = m_PManager->Allocate();
    return AsEnv(cell, bindings, parent);
  }

  CFunction *MakeFunction(CCons *fdef, CEnv *env) {
    CCell *cell = m_PManager->Allocate();
    return AsFunction(cell, fdef, env);
  }

  CFunction *MakeMacro(CCons *fdef, CEnv *env) {
    CCell *cell = m_PManager->Allocate();
    return AsMacro(cell, fdef, env);
  }

  CCell *Apply(CCell *func_expr, CCons *args, CEnv *env) {
    ValueType t = func_expr->Type();

    if (t == LISP_PRIMITIVE) {
      Primitive p = ((CPrimitive *)func_expr)->GetPFunc();
      return p(this, args, env);
    }

    else if (t == LISP_FUNCTION || t == LISP_MACRO) {
      CFunction *f = (CFunction *)func_expr;
      CCons *params = (CCons *)f->Def()->First();
      CCell *fdef = f->Def()->Second();
      CEnv *fdef_env = f->Env();

      CEnv *local_env = BindParameters(params, args, fdef_env);
      return Eval(fdef, local_env);
    } else {
      m_PPrintFer->PrintF("Error Evaling list: bad verb type: %d\n", t);
      return Nil;
    }
  }

  CCell *Let(CCons *name_values, CCell *body, CEnv *env) {
    CEnv *local_env = MakeEnv((CCons *)Nil, env);

    CCons *nv = name_values;
    while (NotNil(nv)) {
      CSymbol *name = (CSymbol *)nv->First();
      nv = nv->Rest();
      if (IsNil(nv)) {
        m_PPrintFer->PrintF("Odd number of values in let.");
      }
      CCell *value = nv->First();
      nv = nv->Rest();
      CCell *evaled_value = Eval(value, local_env);
      Bind(name, evaled_value, local_env);
    }

    return Eval(body, local_env);
  }

  CEnv *BindParameters(CCons *params, CCons *args, CEnv *parent_env = 0);

  void Bind(CSymbol *, CCell *, CEnv *);

  void DefinePrimitive(const char *, Primitive);

  CSymbol *Intern(const char *name);

  CCons *Symbols(void) { return m_Symbols; }

  CEnv *Env(void) { return m_Env; }

  CSymbol *True() { return m_SymTrue; }

  CSymbol *Eof() { return m_SymEof; }

  CSymbol *RParen() { return m_SymRParen; }

  CSymbol *Quote() { return m_SymQuote; }

  CSymbol *Def() { return m_SymDef; }

  CSymbol *SymNil() { return m_SymNil; }

  CPrintFer *GetPrintFer() { return m_PPrintFer; }

  void DebugValue(const char *msg, CCell *value) {
    m_PPrintFer->PrintF("%s: ", msg);
    Print(value);
    m_PPrintFer->PrintF("\n");
  }

  CMemoryManager *Manager() { return m_PManager; }

private:
  bool FindSymbol(const char *name, CSymbol **result);
  CCell *EvalList(CCons *, CEnv *);

private:
  CMemoryManager *m_PManager;
  long m_GCThreshold;
  CPrintFer *m_PPrintFer;
  CLogger *m_PLogger;
  CCons *m_Symbols;
  CEnv *m_Env;
  CInt *m_Ints;
  CSymbol *m_SymTrue;
  CSymbol *m_SymIf;
  CSymbol *m_SymFn;
  CSymbol *m_SymMacro;
  CSymbol *m_SymLet;
  CSymbol *m_SymEof;
  CSymbol *m_SymRParen;
  CSymbol *m_SymQuote;
  CSymbol *m_SymDef;
  CSymbol *m_SymNil;
};
#endif
