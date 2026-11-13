#include "prims_basic.h"
#include "ellipsis.h"
#include "prim_utils.h"

PRIM(FInfo) {
  CKernel *k = GetKernelP();
  k->Print("size of CCell %d\n", sizeof(CCell));
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

PRIM(FReduce) {
  UNPACK2(f, lst)
  return lisp->Reduce(f, (CCons *)lst, env);
}

PRIM(FList) { return args; }

PRIM(FApply) {
  UNPACK2(f, lst)
  return lisp->Apply(f, (CCons *)lst, env);
}

PRIM(FEnv) { return env->GetBindings(); }

PRIM(FFirst) {
  UNPACK1(lst)
  return ((CCons *)lst)->First();
}

PRIM(FRest) {
  UNPACK1(lst)
  return ((CCons *)lst)->Rest();
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

void InitBasicPrimitives(CLisp *lisp) {
  lisp->DefinePrimitive("*info*", FInfo);
  lisp->DefinePrimitive("+", FAdd);
  lisp->DefinePrimitive("-", FSub);
  lisp->DefinePrimitive("*", FMul);
  lisp->DefinePrimitive("/", FDiv);
  lisp->DefinePrimitive("double", FDouble);
  lisp->DefinePrimitive("map", FMap);
  lisp->DefinePrimitive("reduce", FReduce);
  lisp->DefinePrimitive("list", FList);
  lisp->DefinePrimitive("apply", FApply);
  lisp->DefinePrimitive("env", FEnv);
  lisp->DefinePrimitive("first", FFirst);
  lisp->DefinePrimitive("rest", FRest);
  lisp->DefinePrimitive("type", FType);
  lisp->DefinePrimitive("identical?", FIdentical);
  lisp->DefinePrimitive("=", FEqual);
}
