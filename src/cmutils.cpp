
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "cmhead.h"

double angToRad(double angle) {
  return angle * ANG_TO_RAD;
}

/**
 * get the great circle distance of two point with same DEC, and RA difference is 
 * @param dec
 * @param errorRadius
 * @return 
 */
float getAngleFromGreatCircle(double dec, double dist) {
  double rst = acos((cos(dist * ANG_TO_RAD) - pow(sin(dec * ANG_TO_RAD), 2)) /
          pow(cos(dec * ANG_TO_RAD), 2));
  return rst*RAD_TO_ANG;
}

float getLineDistance(CMStar *p1, CMStar *p2) {
  float xdiff = p1->pixx - p2->pixx;
  float ydiff = p1->pixy - p2->pixy;
  float dist = sqrt(xdiff * xdiff + ydiff * ydiff);
  return dist;
}

double getGreatCircleDistance(CMStar *p1, CMStar *p2) {
  double rst = 0.0;
  if (fabs(p1->delta - p2->delta) > CompareFloat || fabs(p1->delta - p2->delta) > CompareFloat) {
    rst = RAD_TO_ANG * acos(sin(ANG_TO_RAD * (p1->delta)) * sin(ANG_TO_RAD * (p2->delta)) +
            cos(ANG_TO_RAD * (p1->delta)) * cos(ANG_TO_RAD * (p2->delta)) * cos(ANG_TO_RAD * (fabs(p1->alpha - p2->alpha))));
  }
  return rst;
}

/**
 * judge a string is a blank string, only contain ' ', '\t','\n','\r',eg
 * @param str
 * @param len
 * @return if it is a blank string return true, others return false
 */
bool isEmpty(char *str) {

  if (NULL != str && '\0' != str[0]) {
    int len = strlen(str);
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
bool hasNumber(char *str) {

  if (NULL != str && '\0' != str[0]) {
    int len = strlen(str);
    for (int i = 0; i < len && '\0' != str[i]; i++) {
      if (str[i] >= '0' && str[i] <= '9') {
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

void ltrim(char *s) {
  char *p;
  p = s;
  while (*p == ' ' || *p == '\t') {
    p++;
  }
  strcpy(s, p);
}

void rtrim(char *s) {
  int i;

  i = strlen(s) - 1;
  while ((s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r' || s[i] == '\x0b') && i >= 0) {
    i--;
  };
  s[i + 1] = '\0';
}

void trim(char *s) {
  ltrim(s);
  rtrim(s);
}

void trimAll(char *s) {
  char *pTmp = s;

  while (*s != '\0') {
    if (*s != ' ' && *s != '\t') {
      *pTmp++ = *s;
    }
    s++;
  }
  *pTmp = '\0';
}

CMStar *CMStar::copy(){
  CMStar *tmp = (CMStar *)malloc(sizeof(CMStar));
  tmp->line = (char*)malloc((strlen(this->line)+1)*sizeof(char));
  strcpy(tmp->line, this->line);
  tmp->alpha = this->alpha;
  tmp->delta = this->delta;
  tmp->error = this->error;
  tmp->id = this->id;
  tmp->mag = this->mag;
  tmp->matchNum = this->matchNum;
  tmp->pixx = this->pixx;
  tmp->pixy = this->pixy;
  tmp->starId = this->starId;
  tmp->next = NULL;
  tmp->match = NULL;
  return tmp;
}