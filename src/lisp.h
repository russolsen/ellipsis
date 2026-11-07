#ifndef _lisp_h_
#define _lisp_h_

#include "io.h"
#include "kernel.h"
#include "manager.h"
#include "value.h"

class CLisp {
public:
  CLisp(long, CPrinter *, CLogger *);

  CCell *Eval(CCell *, CEnv *);

  CCons *EvalItems(CCons *, CEnv *);

  void Print(CCell *expr);

  CCell *Load(const char *);

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

  CCell *MakeInt(long v = 0) {
    if (v <= MAX_CACHED_INT && v >= MIN_CACHED_INT) {
      return m_Ints + (v + CACHED_INT_BIAS - 1);
    }
    CCell *cell = m_PManager->Allocate();
    return AsInt(cell, v);
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
    m_PPrinter->Print("Apply: ");
    Print(args);
    m_PPrinter->Print("\n");

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
      m_PPrinter->Print("Error Evaling list: bad verb type: %d\n", t);
      // func_expr->Print(m_PPrinter);
      // m_PPrinter->Print("\n");
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
        m_PPrinter->Print("Odd number of values in let.");
      }
      CCell *value = nv->First();
      nv = nv->Rest();
      CCell *evaled_value = Eval(value, local_env);
      Bind(name, evaled_value, local_env);
    }

    return Eval(body, local_env);
  }

  CEnv *BindParameters(CCons *params, CCons *args, CEnv *parent_env = 0) {
    CEnv *result = MakeEnv((CCons *)Nil, parent_env);
    for (CCons *pParam = params; NotNil(pParam); pParam = pParam->Rest()) {
      if (args == Nil) {
        m_PPrinter->Print("Too few Arguments!");
        return (CEnv *)Nil; // Error.
      }
      CSymbol *name = (CSymbol *)pParam->First();
      Bind(name, args->First(), result);
      args = args->Rest();
    }
    if (args != Nil) {
      m_PPrinter->Print("Too many Arguments!");
      return (CEnv *)Nil;
    }
    return result;
  }

  void GC(void);

  void Bind(CSymbol *, CCell *, CEnv *);
  void DefinePrimitive(const char *, Primitive);

  CSymbol *Intern(const char *name);

  CCons *Symbols(void) { return m_Symbols; }

  CEnv *Env(void) { return m_Env; }

  CSymbol *Eof() { return m_SymEof; }

  CSymbol *RParen() { return m_SymRParen; }

  CSymbol *Quote() { return m_SymQuote; }

  CSymbol *Def() { return m_SymDef; }

  CSymbol *SymNil() { return m_SymNil; }

  void DebugValue(const char *msg, CCell *value) {
    m_PPrinter->Print("%s: ", msg);
    Print(value);
    m_PPrinter->Print("\n");
  }

private:
  bool FindSymbol(const char *name, CSymbol **result);
  CCell *EvalList(CCons *, CEnv *);

private:
  CMemoryManager *m_PManager;
  CPrinter *m_PPrinter;
  CLogger *m_PLogger;
  CCons *m_Symbols;
  CEnv *m_Env;
  CInt *m_Ints;
  CSymbol *m_SymIf;
  CSymbol *m_SymFn;
  CSymbol *m_SymMacro;
  CSymbol *m_SymLet;
  CSymbol *m_SymEof;
  CSymbol *m_SymRParen;
  CSymbol *m_SymQuote;
  CSymbol *m_SymDef;
  CSymbol *m_SymNil;
  CSymbol *m_SymEnv;
};
#endif
