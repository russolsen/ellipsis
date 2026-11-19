#include "utils.h"
#include "proxydevice.h"

// Function to reverse a string
void ReverseString(char *str){
    size_t left = 0;
    size_t right = strlen(str);

    // Swap characters from both ends
    while (left < right){
        right--;

        char temp = str[left];
        str[left] = str[right];
        str[right] = temp;
        left++;
    }
}

const char *digits = "0123456789";

char *IntToString(int i, char* s) {
  int current = i;

  if (i == 0) {
    strcpy(s, "0");
    return s;
  }

  char *p = s;

  bool neg = false;

  if (current < 0) {
    current *= -1;
    neg = true;
  }

  while(current) {
    int iDigit = current % 10;
    *p++ = digits[iDigit];
    current /= 10;
  }

  if(neg) {
    *p++ = '-';
  }

  *p = 0;
  ReverseString(s);

  return s;
  
}
