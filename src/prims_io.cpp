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

PRIM(FStat) {
  FRESULT fr;
  ARG1_AS(path_string, CCharString *)
  const char *path = path_string->String();

  FILINFO fno;
  fr = f_stat(path, &fno);

  CCell *result = Nil;

  if (FR_OK == fr) {
    CCell *c_size = lisp->MakeInt(fno.fsize);

    char attrs[6];
    strcpy(attrs, "-----");
    if (fno.fattrib & AM_DIR)
      attrs[0] = 'D';
    else
      attrs[0] = 'F';
    if (fno.fattrib & AM_RDO)
      attrs[1] = 'R';
    if (fno.fattrib & AM_HID)
      attrs[2] = 'H';
    if (fno.fattrib & AM_SYS)
      attrs[3] = 'S';
    if (fno.fattrib & AM_ARC)
      attrs[4] = 'A';

    CCell *c_attrs = lisp->MakeString(attrs);

    result = lisp->MakeListZ(path_string, c_attrs, c_size, 0);
  }

  return result;
}

void InitIOPrimitives(CLisp *lisp) {
  lisp->DefinePrimitive("cat", FCat);
  lisp->DefinePrimitive("f-open", FOpen);
  lisp->DefinePrimitive("f-close", FClose);
  lisp->DefinePrimitive("read", FRead);
  lisp->DefinePrimitive("load", FLoad);
  lisp->DefinePrimitive("dir", FDir);
  lisp->DefinePrimitive("stat", FStat);
}
