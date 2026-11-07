#include "ellipsis.h"
#include "fatfiledevice.h"
#include "parser.h"
#include "prim_utils.h"
#include "prims_basic.h"

PRIM(FCat) {
  ARG1_AS(path, CCharString *)
  CKernel *k = GetKernelP();
  CFatFileDevice fd;
  fd.Initialize(path->String(), FA_READ | FA_OPEN_EXISTING);

  char buf[100];
  int n;

  while ((n = fd.Read(buf, sizeof(buf))) > 0) {
    for (int j = 0; j < n; j++) {
      k->Print("%c", buf[j]);
    }
  }

  fd.Close();
  return Nil;
}

PRIM(FOpen) {
  ARG1_AS(path, CCharString *)
  CFatFileDevice *fd = new CFatFileDevice();
  fd->Initialize(path->String(), FA_READ | FA_OPEN_EXISTING);

  return lisp->MakeInt((long)fd);
}

PRIM(FClose) {
  ARG1_INT(i_device);
  CFatFileDevice *fd = (CFatFileDevice *)i_device;
  fd->Close();
  delete fd;
  return Nil;
}

PRIM(FRead) {
  ARG1_AS(path, CCharString *)
  CFatFileDevice fd;
  fd.Initialize(path->String(), FA_READ | FA_OPEN_EXISTING);

  CParser parser(&fd);

  CCell *result = parser.ReadValue(lisp);
  fd.Close();

  return result;
}

PRIM(FLoad) {
  ARG1_AS(path, CCharString *)
  CFatFileDevice fd;
  return lisp->Load(path->String());
}

PRIM(FDir) {

  const char *path = "/";
  const char *pat = "*";

  int n = args->Count();

  if (n >= 1) {
    ARG1_AS(path_string, CCharString *)
    path = path_string->String();
  }

  if (n > 1) {
    ARG2_AS(pat_string, CCharString *)
    pat = pat_string->String();
  }

  CCell *contents = Nil;

  DIR Directory;
  FILINFO FileInfo;
  FRESULT Result = f_findfirst(&Directory, &FileInfo, path, pat);
  for (unsigned i = 0; Result == FR_OK && FileInfo.fname[0]; i++) {
    if (!(FileInfo.fattrib & (AM_HID | AM_SYS))) {
      contents = lisp->Cons(lisp->MakeString(FileInfo.fname), contents);
    }
    Result = f_findnext(&Directory, &FileInfo);
  }
  return contents;
}

void InitIOPrimitives(CLisp *lisp) {
  lisp->DefinePrimitive("cat", FCat);
  lisp->DefinePrimitive("f-open", FOpen);
  lisp->DefinePrimitive("f-close", FClose);
  lisp->DefinePrimitive("read", FRead);
  lisp->DefinePrimitive("load", FLoad);
  lisp->DefinePrimitive("dir", FDir);
}
