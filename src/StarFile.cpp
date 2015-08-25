/* 
 * File:   StarFile.cpp
 * Author: xy
 * 
 * Created on December 4, 2014, 9:15 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "StarFile.h"

StarFile::StarFile(int mode) {
  this->fileName = NULL;
  this->starList = NULL;
  this->starNum = 0;
  this->mode = mode;
}

StarFile::StarFile(char* fileName, int mode) {
  this->fileName = fileName;
  this->starList = NULL;
  this->starNum = 0;
  this->mode = mode;
}

StarFile::StarFile(const StarFile& orig) {

  this->starNum = orig.starNum;
  this->fileName = orig.fileName;

  CMStar *tStar = NULL;
  CMStar *origlst = orig.starList;

  starList = (CMStar *) malloc(sizeof (CMStar));
  memcpy(starList, origlst, sizeof (CMStar));
  starList->line = (char*) malloc(strlen(origlst->line) + 1);
  strcpy(starList->line, origlst->line);
  origlst = origlst->next;
  tStar = starList;

  while (NULL != origlst) {
    tStar->next = (CMStar *) malloc(sizeof (CMStar));
    memcpy(tStar->next, origlst, sizeof (CMStar));
    tStar->next->line = (char*) malloc(strlen(origlst->line) + 1);
    strcpy(tStar->next->line, origlst->line);
    origlst = origlst->next;
    tStar = tStar->next;
  }
}

StarFile::~StarFile() {

  if (NULL == starList) {
    return;
  }

  CMStar *tStar = starList->next;
  while (tStar) {
    starList->next = tStar->next;
    free(tStar->line);
    free(tStar);
    tStar = starList->next;
  }
  free(starList);
}

void StarFile::readStar() {
  readStar(fileName);
}

/**
 * 
 * @param fName
 * @param idxs三个列的序号，分别为x(ra),y(dec),mag。 默认为NULL时，自动读取前三列；如果只有前两列，则第三个数为0
 * @param pnum读入数据的总列数
 * @return 
 */
void StarFile::readStar(char * fileName, int *idxs, int pnum) {

  if (NULL == fileName) {
    printf("file name is NULL!\n");
    return;
  }

  FILE *fp = fopen(fileName, "r");

  if (NULL == fp) {
    printf("cannot open file %s!\n", fileName);
    return;
  }

  starNum = 0;
  CMStar *tStar = NULL;
  CMStar *nextStar = NULL;
  char line[MaxStringLength];
  float data1 = 0.0;
  float data2 = 0.0;
  float mag = 0.0;

  char farmatStr[255] = {0};
  if (idxs == NULL) {
    sprintf(farmatStr, "%%f%%f%%f");
  } else {
    for (int i = 1; i <= pnum; i++) {
      if (idxs[0] == i || idxs[1] == i || idxs[2] == i) {
        sprintf(farmatStr, "%s%%f", farmatStr);
      } else {
        sprintf(farmatStr, "%s%%*s", farmatStr);
      }
    }
  }

  int readNum = 3;
  if (idxs[2] == 0) {//不读mag值
    readNum = 2;
  }

  while (fgets(line, MaxStringLength, fp) != NULL) {
    if (readNum == sscanf(line, farmatStr, &data1, &data2, &mag)) {
      nextStar = (CMStar *) malloc(sizeof (CMStar));
      nextStar->id = starNum;
      if (mode == 0) {
        nextStar->pixx = data1;
        nextStar->pixy = data2;
      } else {
        nextStar->alpha = data1;
        nextStar->delta = data2;
      }
      nextStar->mag = mag;
      nextStar->next = NULL;
      nextStar->match = NULL;
      nextStar->line = (char*) malloc(strlen(line) + 1);
      strcpy(nextStar->line, line);
      if (NULL == starList) {
        starList = nextStar;
        tStar = nextStar;
      } else {
        tStar->next = nextStar;
        tStar = nextStar;
      }
      starNum++;
    }
  }

#ifdef PRINT_CM_DETAIL
  printf("%s read %d stars\n", fileName, starNum);
#endif
}

void StarFile::writeStar(char * outFile) {

  FILE *fp = fopen(outFile, "w");

  long count = 0;
  CMStar *tStar = starList;
  while (NULL != tStar) {
    fprintf(fp, "%%f", tStar->line);
    count++;
    tStar = tStar->next;
  }
  fclose(fp);

#ifdef PRINT_CM_DETAIL
  printf("total write %d lines.\n", count);
#endif
}