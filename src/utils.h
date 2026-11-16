#ifndef _utils_h
#define _utils_h

inline int IntMax(int a, int b) {
  if (a > b)
    return a;
  return b;
}

inline bool IsDigit(int ch) { return ch >= '0' && ch <= '9'; }

inline bool IsOctalDigit(int ch) { return ch >= '0' && ch <= '7'; }

inline bool IsHexDigit(int ch) {
  return IsDigit(ch) || (ch >= 'a' && ch <= 'f');
}

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

inline bool IsLowerCase(int ch) { return ch >= 'a' && ch <= 'z'; }
inline bool IsUpperCase(int ch) { return ch >= 'A' && ch <= 'Z'; }

inline int ToLower(int ch) {
  if (IsUpperCase(ch)) {
    int ord = ch - 'A';
    return 'a' + ord;
  }
  return ch;
}

inline int ToUpper(int ch) {
  if (IsLowerCase(ch)) {
    int ord = ch - 'a';
    return 'A' + ord;
  }
  return ch;
}
#endif
