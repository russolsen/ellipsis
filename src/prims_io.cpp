#include "ellipsis.h"
#include "fatfiledevice.h"
#include "parser.h"
#include "prim_utils.h"
#include "prims_io.h"

PRIM(FPrint) {
  for (CCons *p = args; p; p = p->Rest()) {
    lisp->GetPrintFer()->PrintF("%v", p->First());
  }
  return Nil;
}

PRIM(FEmit) {
  for (CCons *p = args; p; p = p->Rest()) {
    CCharString *s = (CCharString *)p->First();
    lisp->GetPrintFer()->PrintF("%s", s->String());
  }

  return Nil;
}

PRIM(FCat) {
  UNPACK1_STR(path)
  CFatFileDevice fd;
  fd.Initialize(path->String(), FA_READ | FA_OPEN_EXISTING);

  char buf[100];
  int n;

  while ((n = fd.Read(buf, sizeof(buf))) > 0) {
    for (int j = 0; j < n; j++) {
      lisp->GetPrintFer()->PrintF("%c", buf[j]);
    }
  }

  fd.Close();
  return Nil;
}

PRIM(FFOpen) {
  UNPACK1_STR(path)
  CFatFileDevice *fd = new CFatFileDevice();
  fd->Initialize(path->String(), FA_READ | FA_OPEN_EXISTING);
  lisp->GetPrintFer()->PrintF("FOpen: fd: %x ", fd);

  CCharReader *cr = (CCharReader *)fd;
  lisp->GetPrintFer()->PrintF("FOpen: cr: %x ", cr);
  return lisp->MakeInt((long)cr);
}

PRIM(FFClose) {
  UNPACK1_INT(i_device);
  CFatFileDevice *fd = (CFatFileDevice *)(i_device->GetInt());
  lisp->GetPrintFer()->PrintF("FClose: fd: %x ", fd);
  fd->Close();
  delete fd;
  return Nil;
}

PRIM(FFRead) {
  UNPACK1_INT(i_device);
  CCharReader *cr = (CCharReader *)(i_device->GetInt());

  CParser parser(cr);
  CCell *result = parser.ReadValue(lisp);

  return result;
}


PRIM(FRead) {
  UNPACK1_STR(path)
  CFatFileDevice fd;
  fd.Initialize(path->String(), FA_READ | FA_OPEN_EXISTING);

  CParser parser(&fd);

  CCell *result = parser.ReadValue(lisp);
  fd.Close();

  return result;
}

PRIM(FLoad) {
  UNPACK1_STR(path)
  CFatFileDevice fd;
  return lisp->Load(path->String());
}

PRIM(FDir) {
  const char *path = "/";
  const char *pat = "*";

  int n = ARG_COUNT();

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
  UNPACK1_STR(path_string)
  const char *path = path_string->String();

  FILINFO fno;
  FRESULT fr = f_stat(path, &fno);

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
  lisp->DefinePrimitive("print", FPrint);
  lisp->DefinePrimitive("emit", FEmit);
  lisp->DefinePrimitive("cat", FCat);
  lisp->DefinePrimitive("f-open", FFOpen);
  lisp->DefinePrimitive("f-read", FFRead);
  lisp->DefinePrimitive("f-close", FFClose);
  lisp->DefinePrimitive("read", FRead);
  lisp->DefinePrimitive("load", FLoad);
  lisp->DefinePrimitive("dir", FDir);
  lisp->DefinePrimitive("stat", FStat);
}
