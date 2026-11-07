#include "parser.h"
#include "ellipsis.h"
#include <circle/util.h>

CParser::CParser(CCharReader *char_reader, bool debug) {
  m_CharReader = char_reader;
  m_Debug = debug;
  m_Pushed = -1;
}

CCell *CParser::ReadValue(CLisp *lisp) {
  TokenType tt = ReadToken();
  CCell *result = lisp->Eof();

  switch (tt) {
  case TT_EOF:
    result = lisp->Eof();
    break;
  case TT_NUMBER:
    result = lisp->MakeInt(atoi(m_Token));
    break;
  case TT_QUOTE:
    result = lisp->MakeListZ(lisp->Quote(), ReadValue(lisp), 0);
    break;
  case TT_SYMBOL:
    result = lisp->Intern(m_Token);
    break;
  case TT_STRING:
    result = lisp->MakeString(m_Token);
    break;
  case TT_LPAREN:
    result = ReadList(lisp);
    break;
  case TT_RPAREN:
    result = lisp->Intern(m_Token);
    break;
  }

  return result;
}

CCons *CParser::ReadList(CLisp *lisp) {
  CCons *result = (CCons *)Nil;
  CCons *tail = (CCons *)Nil;
  CCell *v = ReadValue(lisp);

  while (v && (v != lisp->RParen()) && (v != lisp->Eof())) {
    CCons *new_link = lisp->Cons(v);
    if (IsNil(result)) {
      result = tail = new_link;
    } else {
      tail->SetRest(new_link);
      tail = new_link;
    }
    v = ReadValue(lisp);
  }
  return result;
}

TokenType CParser::ReadToken(void) {
  SkipSpace();
  m_Token[0] = 0;

  int ch = PeekChar();

  if (m_Debug)
    Print("1st char is %c (%d)\n", ch, ch);

  if (ch <= 0) {
    return TT_EOF;
  }

  if (ch == '(' || ch == ')' || ch == '\'') {
    m_Token[0] = GetChar();
    m_Token[1] = 0;
    return (TokenType)ch;
  }

  if (ch == '"') {
    ReadString();
    return TT_STRING;
  }

  if (IsDigit(ch) || ch == '+' || ch == '-') {
    ReadDelimited();
    // Ugly special case to catch + and - symbols.
    if ((strcmp(m_Token, "+") == 0) || ((strcmp(m_Token, "-") == 0))) {
      return TT_SYMBOL;
    }
    return TT_NUMBER;
  }

  ReadDelimited();
  return TT_SYMBOL;
}

int CParser::GetChar(void) {
  int result = m_Pushed;
  if (result >= 0) {
    m_Pushed = -1;
    return result;
  }
  return m_CharReader->GetChar();
}

void CParser::UnGetChar(int ch) { m_Pushed = ch; }

int CParser::PeekChar(void) {
  int ch = GetChar();
  UnGetChar(ch);
  return ch;
}

void CParser::SkipSpace() {
  int ch = GetChar();
  while (IsSpace(ch)) {
    ch = GetChar();
  }
  if (ch > 0) {
    UnGetChar(ch);
  }
}

char *CParser::ReadDelimited() {
  int i = 0;
  int ch = GetChar();
  while (ch > 0) {
    if (IsSpace(ch))
      break;
    if (ch == '(' || ch == ')' || ch == '\'') {
      UnGetChar(ch);
      break;
    }
    m_Token[i++] = ch;
    ch = GetChar();
  }
  m_Token[i] = 0;
  return m_Token;
}

char *CParser::ReadString() {
  int quote = GetChar();

  int i = 0;
  int ch = GetChar();
  while (ch != quote) {
    if (ch <= 0) {
      break;
    } else if (ch == '\\') {
      ch = GetChar();
      if (ch == 'n')
        m_Token[i++] = '\n';
      else if (ch == 't')
        m_Token[i++] = '\t';
      else
        m_Token[i++] = ch;
    } else {
      m_Token[i++] = ch;
    }
    ch = GetChar();
  }

  m_Token[i] = 0;
  return m_Token;
}
