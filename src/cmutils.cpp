
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "cmhead.h"

float getLineDistance(cm_star *p1, cm_star *p2) {
  float xdiff = p1->pixx - p2->pixx;
  float ydiff = p1->pixy - p2->pixy;
  float dist = sqrt(xdiff * xdiff + ydiff * ydiff);
  return dist;
}

/**
 * judge a string is a blank string, only contain ' ', '\t','\n','\r',eg
 * @param str
 * @param len
 * @return if it is a blank string return true, others return false
 */
bool isEmpty(char *str, int len) {

  if (NULL != str && '\0' != str[0]) {
    for (int i = 0; i < len && '\0' != str[i]; i++) {
      if (str[i] != ' ' && str[i] != '\t' && str[i] != '\r' && 
              str[i] != '\n' && str[i] != '\x0b') {
        return false;
      }
    }
  }
  return true;
}

/**
 * judge a string whether contain a number char ('0'~"9")
 * @param str
 * @param len
 * @return 
 */
bool hasNumber(char *str, int len) {
  
  if (NULL != str && '\0' != str[0]) {
    for (int i = 0; i < len && '\0' != str[i]; i++) {
      if (str[i] >='0' && str[i] <= '9') {
        return true;
      }
    }
  }
  return false;
}

template<typename T>
inline T getMax(const T& a, const T& b) {
  return a > b ? a : b;
}

long countFileLines(char *fName) {

  FILE *fp = fopen(fName, "r");

  if (fp == NULL) {
    return 0;
  }

  long lineNum = 0;
  char line[MaxStringLength];
  while (fgets(line, MaxStringLength, fp) != NULL) {
    lineNum++;
  }
  fclose(fp);
  return lineNum;
}