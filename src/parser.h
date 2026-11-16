#include "lisp.h"
#include "utils.h"

enum TokenType : char {
  TT_EOF = '!',
  TT_SYMBOL = 'S',
  TT_NUMBER = '9',
  TT_LPAREN = '(',
  TT_RPAREN = ')',
  TT_QUOTE = '\'',
  TT_STRING = '"',
};

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

  void SkipWhiteSpace();

  void SkipEOL();

  char *ReadDelimited();

  char *ReadString();

  char ReadOctalChar();

  char ReadHexChar();

private:
  CCharReader *m_CharReader;
  char m_Token[256];
  int m_Pushed;
  bool m_Debug;
};
