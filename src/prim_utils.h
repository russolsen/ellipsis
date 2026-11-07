#ifndef __prim_utils_h
#define __prim_utils_h

#define PRIM(_fname) CCell *_fname(CLisp *lisp, CCons *args, CEnv *env)

#define CK_ARGS(_n)                                                            \
  if (args->Count() != _n) {                                                   \
    return Nil;                                                                \
  }

#define NO_ARGS() CK_ARGS(0)

#define UNPACK1(_name1)                                                        \
  CK_ARGS(1)                                                                   \
  CCell *_name1 = args->First();

#define ARG1_AS(_name, _type) _type _name = (_type)args->First();
#define ARG2_AS(_name, _type) _type _name = (_type)args->Second();

#define ARG1_INT(_name) ARG1_AS(_name, CInt *)

#define UNPACK2(_name1, _name2)                                                \
  CK_ARGS(2)                                                                   \
  CCell *_name1 = args->First();                                               \
  CCell *_name2 = args->Second();

#define UNPACK2_INTS(_name1, _name2)                                           \
  CK_ARGS(2)                                                                   \
  CInt *_name1 = (CInt *)args->First();                                        \
  CInt *_name2 = (CInt *)args->Second();

#endif
