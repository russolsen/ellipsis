#include <circle/devicenameservice.h>
#include "prims_basic.h"
#include "ellipsis.h"
#include "prim_utils.h"

PRIM(FGC) {
  lisp->DoGC();
  return Nil;
}

PRIM(FFreeCount) {
  return lisp->MakeInt(lisp->Manager()->FreeCount());
}

PRIM(FInfo) {
  CKernel *k = GetKernelP();
  k->Print("Size of CCell: %d ", sizeof(CCell));
  k->Print("CFunction: %d  ", sizeof(CFunction));
  k->Print("CCons: %d ", sizeof(CCons));
  k->Print("CPrimitive: %d ", sizeof(CPrimitive));
  k->Print("CEnv: %d ", sizeof(CEnv));
  k->Print("\n");
  k->Print("Num free cells: %d\n", lisp->Manager()->FreeCount());
  k->Print("\n");
  k->Print("Devices:\n");
  CDeviceNameService *dns = CDeviceNameService::Get();
  dns->ListDevices(k->GetConsoleP(), true);
  return Nil;
}

PRIM(FAdd) {
  UNPACK2_INTS(a, b)
  return lisp->MakeInt(a->GetInt() + b->GetInt());
}

PRIM(FSub) {
  UNPACK2_INTS(a, b)
  return lisp->MakeInt(a->GetInt() - b->GetInt());
}

PRIM(FMul) {
  UNPACK2_INTS(a, b)
  return lisp->MakeInt(a->GetInt() * b->GetInt());
}

PRIM(FDiv) {
  UNPACK2_INTS(a, b)
  return lisp->MakeInt(a->GetInt() / b->GetInt());
}

PRIM(FDouble) {
  UNPACK1(arg)
  int a = ((CInt *)arg)->GetInt();
  return lisp->MakeInt(a * 2);
}

PRIM(FMap) {
  UNPACK2(f, lst)
  return lisp->Map(f, (CCons *)lst, env);
}

PRIM(FDo) {
  return args->Last();
}

PRIM(FDoAll) {
  UNPACK2(f, lst)
  CCell *result = Nil;

  for (CCons *l = (CCons *)lst; l; l = l->Rest()) {
    result = lisp->Apply(f, lisp->Cons(l->First()), env);
  }
  return result;
}

PRIM(FReduce) {
  UNPACK2(f, lst)
  return lisp->Reduce(f, (CCons *)lst, env);
}

PRIM(FList) { return args; }

PRIM(FApply) {
  UNPACK2(f, lst)
  return lisp->Apply(f, (CCons *)lst, env);
}

PRIM(FBaseEnv) { return lisp->Env()->GetBindings(); }

PRIM(FEnv) { return env->GetBindings(); }

PRIM(FFirst) {
  UNPACK1(lst)
  return ((CCons *)lst)->First();
}

PRIM(FRest) {
  UNPACK1(lst)
  return ((CCons *)lst)->Rest();
}

PRIM(FSecond) {
  UNPACK1(lst)
  return ((CCons *)lst)->Second();
}


PRIM(FThird) {
  UNPACK1(lst)
  return ((CCons *)lst)->Third();
}

PRIM(FType) {
  UNPACK1(v)
  return lisp->MakeInt(TypeOf(v));
}

PRIM(FIdentical) {
  UNPACK2(a, b)
  return (a == b) ? lisp->True() : Nil;
}

PRIM(FEqual) {
  UNPACK2(a, b)
  if (a == b)
    return lisp->True();

  return lisp->Equal(a, b);
}

PRIM(FDefinedQ) {
  CK_ARGS(1)
  ARG1_AS(sym, CSymbol *)

  return lisp->MakeBool(env->FindBinding(sym));
}


PRIM(FStrlen) {
  CK_ARGS(1)
  ARG1_AS(s, CCharString *)

  int n = strlen(s->String());
  return lisp->MakeInt(n);
}

void InitBasicPrimitives(CLisp *lisp) {
  lisp->DefinePrimitive("gc", FGC);
  lisp->DefinePrimitive("free-count", FFreeCount);
  lisp->DefinePrimitive("*info*", FInfo);
  lisp->DefinePrimitive("+", FAdd);
  lisp->DefinePrimitive("-", FSub);
  lisp->DefinePrimitive("*", FMul);
  lisp->DefinePrimitive("/", FDiv);
  lisp->DefinePrimitive("double", FDouble);
  lisp->DefinePrimitive("map", FMap);
  lisp->DefinePrimitive("do", FDo);
  lisp->DefinePrimitive("doall", FDoAll);
  lisp->DefinePrimitive("reduce", FReduce);
  lisp->DefinePrimitive("list", FList);
  lisp->DefinePrimitive("apply", FApply);
  lisp->DefinePrimitive("*env*", FBaseEnv);
  lisp->DefinePrimitive("env", FEnv);
  lisp->DefinePrimitive("first", FFirst);
  lisp->DefinePrimitive("rest", FRest);
  lisp->DefinePrimitive("second", FSecond);
  lisp->DefinePrimitive("third", FThird);
  lisp->DefinePrimitive("type", FType);
  lisp->DefinePrimitive("identical?", FIdentical);
  lisp->DefinePrimitive("=", FEqual);
  lisp->DefinePrimitive("defined?", FDefinedQ);
  lisp->DefinePrimitive("strlen", FStrlen);
}
