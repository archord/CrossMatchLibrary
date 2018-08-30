/* 
 * File:   StarClassify.cpp
 * Author: xy
 * 
 * Created on March 11, 2014, 9:57 AM
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cmhead.h"
#include "StarFile.h"
#include "CrossMatchSelf2.h"

using namespace std;

void CrossMatchSelf2::printMatched(char *outfName, CMStar *tStar, float errorBox) {

  FILE *fp = fopen(outfName, "w");

  long count = 0;
  while (NULL != tStar) {
    if (tStar->matchNum > 1) {
      fprintf(fp, "%s", tStar->line);
      count++;
    }
    tStar = tStar->next;
  }
  fclose(fp);

#ifdef PRINT_CM_DETAIL
  printf("matched stars %d\n", count);
#endif
}

void CrossMatchSelf2::printNotMatched(char *outfName, CMStar *tStar) {

  FILE *fp = fopen(outfName, "w");

  long count = 0;
  while (NULL != tStar) {
    if (tStar->matchNum <= 1) {
      fprintf(fp, "%s", tStar->line);
      count++;
    }
    tStar = tStar->next;
  }
  fclose(fp);

#ifdef PRINT_CM_DETAIL
  printf("OT stars %d\n", count);
#endif
}

/**
 * circle(1298.45614218,3529.17511152,8.4547586441) # color=green width=2 text={12} font="times 7"
 * @param outfName
 * @param starFile
 * @param errorBox
 */
void CrossMatchSelf2::printMatchedDs9(char *outfName, CMStar *tStar, float errorBox) {

  FILE *fp = fopen(outfName, "w");

  long count = 0;
  while (NULL != tStar) {
    if (tStar->matchNum > 1) {
      fprintf(fp, "image;circle(%.2f,%.2f,%.2f) # color=green width=1 text={%d-%.2f} font=\"times 7\"\n",
              tStar->pixx, tStar->pixy, 4.0, tStar->id, tStar->mag);
      count++;
    }
    tStar = tStar->next;
  }
  fclose(fp);

#ifdef PRINT_CM_DETAIL
  printf("matched stars %d\n", count);
#endif
}

void CrossMatchSelf2::printNotMatchedDs9(char *outfName, CMStar *tStar) {

  FILE *fp = fopen(outfName, "w");

  long count = 0;
  while (NULL != tStar) {
    if (tStar->matchNum <= 1) {
      fprintf(fp, "image;circle(%.2f,%.2f,%.2f) # color=green width=1 text={%ld-%.2f} font=\"times 7\"\n",
              tStar->pixx, tStar->pixy, 4.0, tStar->id, tStar->mag);
      count++;
    }
    tStar = tStar->next;
  }
  fclose(fp);

#ifdef PRINT_CM_DETAIL
  printf("OT stars %d\n", count);
#endif
}

