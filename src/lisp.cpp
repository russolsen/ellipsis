#include "lisp.h"
#include "fatfiledevice.h"
#include "kernel.h"
#include "parser.h"
#include "prims_basic.h"
#include "prims_io.h"
#include <circle/stdarg.h>

const char *FromLisp = "Lisp";

CLisp::CLisp(long msize, CPrinter *printer, CLogger *logger) {
  m_PManager = new CMemoryManager(msize);
  m_PPrinter = printer;
  m_PLogger = logger;
  m_Symbols = (CCons *)Nil;

  m_Ints = new CInt[MAX_CACHED_INT];

  for (int i = 0; i < N_CACHED_INTS; i++) {
    m_Ints[i].Morph(i - CACHED_INT_BIAS + 1);
  }

  CEnv *super_env = MakeEnv();
  Bind(Intern("Lang"), MakeString("Ellipsis"), super_env);

  m_Env = MakeEnv((CCons *)Nil, super_env);

  m_SymTrue = Intern("#t");
  m_SymIf = Intern("if");
  m_SymFn = Intern("fn");
  m_SymMacro = Intern("macro");
  m_SymLet = Intern("let");
  m_SymQuote = Intern("quote");
  m_SymDef = Intern("def");
  m_SymEof = Intern("*eof*");
  m_SymRParen = Intern(")");
  m_SymNil = Intern("nil");
  m_SymEnv = Intern("*env*");

  Bind(Intern("version"), MakeInt(100), m_Env);
  Bind(m_SymTrue, m_SymTrue, m_Env);
  Bind(m_SymNil, Nil, m_Env);
  InitBasicPrimitives(this);
  InitIOPrimitives(this);
}

CCell *CLisp::Eval(CCell *expr, CEnv *env = 0) {
  // m_PPrinter->Print("Eval: ");
  // Print(expr);
  // m_PPrinter->Print("\n");

  if (!env)
    env = m_Env;

  CCons *b = (CCons *)Nil;
  ValueType t = TypeOf(expr);

  switch (t) {
  case LISP_NIL:
  case LISP_INTEGER:
  case LISP_PRIMITIVE:
  case LISP_STRING:
  case LISP_SHORT_STRING:
  case LISP_FUNCTION:
  case LISP_MACRO:
    return expr;

  case LISP_SYMBOL:
  case LISP_SHORT_SYMBOL:
    if (!env->FindBinding(expr, &b)) {
      m_PPrinter->Print("%s: ???\n", ((CSymbol *)expr)->String());
      return Nil;
    }
    return b->Cdr();

  case LISP_CONS:
    return EvalList((CCons *)expr, env);
  default:
    m_PPrinter->Print("%x: ???\n", expr);
    return Nil;
  }
}

void CLisp::Print(CCell *expr) {
  ValueType t = TypeOf(expr);

  switch (t) {
  case LISP_NIL:
    m_PPrinter->Print("nil ");
    break;
  case LISP_INTEGER:
    m_PPrinter->Print("%d ", ((CInt *)expr)->GetInt());
    break;
  case LISP_PRIMITIVE:
    m_PPrinter->Print("Prim(%x) ", ((CPrimitive *)expr)->GetPFunc());
    break;
  case LISP_STRING:
  case LISP_SHORT_STRING:
    m_PPrinter->Print("\"%s\" ", ((CCharString *)expr)->String());
    break;
  case LISP_FUNCTION:
  case LISP_MACRO:
    if (t == LISP_MACRO)
      m_PPrinter->Print("(*macro ");
    else
      m_PPrinter->Print("(*fn ");
    Print(((CFunction *)expr)->Def()->First());
    Print(((CFunction *)expr)->Def()->Second());
    m_PPrinter->Print(") ");
    break;
  case LISP_SYMBOL:
  case LISP_SHORT_SYMBOL:
    m_PPrinter->Print("%s ", ((CSymbol *)expr)->String());
    break;
  case LISP_CONS:
    if (IsProperList(expr)) {
      m_PPrinter->Print("(");
      for (CCons *p = (CCons *)expr; p; p = p->Rest()) {
        Print(p->First());
        m_PPrinter->Print(" ");
      }
      m_PPrinter->Print(")");
    } else {
      m_PPrinter->Print("[");
      Print(((CCons *)expr)->First());
      m_PPrinter->Print(".");
      Print(((CCons *)expr)->Rest());
      m_PPrinter->Print("]");
    }
    break;
  default:
    m_PPrinter->Print("%x?? ", expr);
    break;
  }
}

CCell *CLisp::EvalList(CCons *expr, CEnv *env) {
  // DebugValue("Eval list:", expr);

  CCell *verb = expr->First();
  CCons *args = expr->Rest();

  if (verb == m_SymIf) {
    CCell *cond = args->First();
    CCell *tpart = args->Second();
    CCell *fpart = args->Third();
    if (IsTruthy(Eval(cond, env)))
      return Eval(tpart, env);
    else
      return Eval(fpart, env);
  } else if (verb == m_SymQuote) {
    return args->First();
  } else if (verb == m_SymLet) {
    return Let(args->First(), args->Second(), env);
  } else if (verb == m_SymFn) {
    return MakeFunction(args, env);
  } else if (verb == m_SymMacro) {
    return MakeMacro(args, env);
  } else if (verb == m_SymEnv) {
    Print(env);
    return Nil;
  } else if (verb == m_SymDef) {
    CSymbol *name = (CSymbol *)args->First();
    CCell *value = Eval(args->Second(), env);
    Bind(name, value, m_Env);
    return name;
  }

  CCell *evaled_verb = Eval(verb, env);
  if (TypeOf(evaled_verb) == LISP_MACRO) {
    CCell *resulting_code = Apply(evaled_verb, args, env);
    // DebugValue("Macro resulting code", resulting_code);
    return Eval(resulting_code, ((CFunction *)evaled_verb)->Env());
  } else {
    // m_PPrinter->Print("Function!\n");
    CCons *evaled_args = EvalItems(args, env);
    return Apply(evaled_verb, evaled_args, env);
  }
}

CCons *CLisp::EvalItems(CCons *expr, CEnv *env) {
  CCons *result = (CCons *)Nil;
  CCons *tail = (CCons *)Nil;

  for (CCons *p = expr; NotNil(p); p = p->Rest()) {
    CCell *new_value = Eval(p->First(), env);
    if (IsNil(result)) {
      result = Cons(new_value, Nil);
      tail = result;
    } else {
      CCons *new_link = Cons(new_value, Nil);
      tail->SetRest(new_link);
      tail = new_link;
    }
  }
  return result;
}

CCell *CLisp::Load(const char *path) {
  CFatFileDevice fd;
  fd.Initialize(path, FA_READ | FA_OPEN_EXISTING);
  CBufferedCharReader reader(&fd);
  CParser parser(&reader, false);

  CCell *result = nullptr;

  CCell *expr = parser.ReadValue(this);
  while (expr != Eof()) {
    result = Eval(expr);
    expr = parser.ReadValue(this);
  }
  fd.Close();
  return result;
}

CCell *CLisp::Equal(CCell *a, CCell *b) {
  const char *s_a, *s_b;
  if (a == b)
    return Nil;

  ValueType t_a = TypeOf(a);
  ValueType t_b = TypeOf(b);

  if (t_a != t_b)
    return Nil;

  switch (t_a) {
  case LISP_INTEGER:
    return (((CInt *)a)->GetInt()) == (((CInt *)b)->GetInt()) ? m_SymTrue : Nil;

  case LISP_STRING:
    s_a = ((CCharString *)a)->String();
    s_b = ((CCharString *)b)->String();
    return strcmp(s_a, s_b) ? Nil : m_SymTrue;

  default:
    return Nil;
  }
}

CCons *CLisp::Map(CCell *f, CCons *lst, CEnv *env) {
  CCons *result = (CCons *)Nil;
  CCons *tail = (CCons *)Nil;

  for (CCons *p = lst; NotNil(p); p = p->Rest()) {
    CCell *new_value = Apply(f, Cons(p->First()), env);
    if (IsNil(result)) {
      result = Cons(new_value, Nil);
      tail = result;
    } else {
      CCons *new_link = Cons(new_value, Nil);
      tail->SetRest(new_link);
      tail = new_link;
    }
  }
  return result;
}

CCell *CLisp::Reduce(CCell *f, CCons *lst, CEnv *env) {
  if (IsNil(lst))
    return lst;

  if (IsNil(lst->Rest()))
    return lst->First();

  CCell *result = lst->First();

  for (CCons *p = lst->Rest(); NotNil(p); p = p->Rest()) {
    CCons *args = MakeList2(result, p->First());
    result = Apply(f, args, env);
  }
  return result;
}

CCons *CLisp::MakeList2(CCell *a, CCell *b) { return Cons(a, Cons(b, Nil)); }

CCons *CLisp::MakeListZ(CCell *first_arg, ...) {
  CCons *result = Cons(first_arg);
  CCons *tail = result;

  va_list args;
  va_start(args, first_arg);

  CCell *arg = va_arg(args, CCell *);
  while (arg) {
    CCons *link = Cons(arg);
    tail->SetRest(link);
    tail = link;
    arg = va_arg(args, CCell *);
  }

  va_end(args);
  return result;
}

void CLisp::GC(void) {
  m_PManager->Mark(m_Symbols);
  m_PManager->Mark(m_Env);
  m_PManager->Sweep();
}

void CLisp::Bind(CSymbol *sym, CCell *value, CEnv *env) {
  CCons *old_bindings = env->GetBindings();
  CCons *new_binding = Cons(sym, value);
  CCons *new_bindings = Cons(new_binding, old_bindings);
  env->SetBindings(new_bindings);
}

void CLisp::DefinePrimitive(const char *name, Primitive prim) {
  Bind(Intern(name), MakePrimitive(prim), m_Env);
}

CSymbol *CLisp::Intern(const char *name) {

  CSymbol *result = (CSymbol *)Nil;

  if (FindSymbol(name, &result)) {
    return result;
  }

  result = MakeSymbol(name);
  m_Symbols = Cons(result, m_Symbols);

  return result;
}

bool CLisp::FindSymbol(const char *name, CSymbol **result) {
  for (CCons *p = m_Symbols; NotNil(p); p = p->Rest()) {
    CSymbol *sym = (CSymbol *)p->First();
    if (strcmp(name, sym->String()) == 0) {
      *result = sym;
      return true;
    }
  }
  return false;
}
