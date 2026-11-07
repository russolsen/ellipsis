//#include <cctype>
//#include <cstdio>
//#include <cstdlib>
//#include <iostream>

#include "lisp.h"

enum TokenType : char {
  TT_EOF = '!',
  TT_SYMBOL = 'S',
  TT_NUMBER = '9',
  TT_LPAREN = '(',
  TT_RPAREN = ')',
  TT_QUOTE = '\'',
  TT_STRING = '"',
};

inline bool IsDigit(int ch) { return ch >= '0' && ch <= '9'; }

inline bool IsSpace(int ch) {
  switch (ch) {
  case ' ':
  case '\n':
  case '\t':
  case '\r':
    return 1;
  }
  return 0;
}

class CParser {

public:
  CParser(CCharReader *, bool debug = false);

  CCell *ReadValue(CLisp *lisp);

private:
  CCons *ReadList(CLisp *lisp);

  TokenType ReadToken(void);

  int GetChar(void);

  void UnGetChar(int ch);

  int PeekChar(void);

  void SkipSpace();

  char *ReadDelimited();

  char *ReadString();

private:
  CCharReader *m_CharReader;
  char m_Token[256];
  int m_Pushed;
  bool m_Debug;
};
