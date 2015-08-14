/* 
 * File:   CrossMatch.cpp
 * Author: xy
 * 
 * Created on October 18, 2013, 8:48 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "CrossMatchSphere.h"

CrossMatchSphere::CrossMatchSphere() {
  refStarFile = NULL;
  objStarFile = NULL;
  refStarFileNoPtn = NULL;
  objStarFileNoPtn = NULL;
  zones = NULL;
}

CrossMatchSphere::CrossMatchSphere(const CrossMatchSphere& orig) {
}

CrossMatchSphere::~CrossMatchSphere() {
}

void CrossMatchSphere::match(char *refName, char *objName, float errorBox) {

  errorBox = errorBox / 3600.0;

  float minZoneLen = errorBox * TimesOfErrorRadius;
  float searchRds = errorBox;

  refStarFile = new StarFile();
  refStarFile->readStar(refName);
  objStarFile = new StarFile();
  objStarFile->readStar(objName);

  zones = new PartitionSphere(errorBox, minZoneLen, searchRds);
  zones->partitonStarField(refStarFile);
  refStarFile->starList = NULL;

  int i = 0;
  CMStar *nextStar = objStarFile->starList;
  while (nextStar) {
    zones->getMatchStar(nextStar);
    nextStar = nextStar->next;
    i++;
  }
  //  printf("i=%d\n", i);

#ifdef PRINT_CM_DETAIL
  printf("partition match done!\n");
#endif
}

/**
 * circulate each star on 'refList', find the nearest on as the match star of objStar
 * the matched star is stored on obj->match, 
 * the distance between two stars is stored on obj->error
 * @param ref
 * @param obj
 */
void CrossMatchSphere::matchNoPartition(char *refName, char *objName, float errorBox) {

  refStarFileNoPtn = new StarFile();
  refStarFileNoPtn->readStar(refName);
  objStarFileNoPtn = new StarFile();
  objStarFileNoPtn->readStar(objName);

  matchNoPartition(refStarFileNoPtn, objStarFileNoPtn, errorBox);

}

/**
 * the matched star is stored on obj->match, 
 * the distance between two stars is stored on obj->error
 * @param ref
 * @param obj
 */
void CrossMatchSphere::matchNoPartition(StarFile *refStarFileNoPtn, StarFile *objStarFileNoPtn, float errorBox) {

  CMStar *tObj = objStarFileNoPtn->starList;

  while (tObj) {
    CMStar *tRef = refStarFileNoPtn->starList;
    float tError = 0.0;
    float minError = getLineDistance(tRef, tObj);
    tObj->match = tRef;
    tObj->error = minError;
    tRef = tRef->next;
    while (tRef) {
      tError = getLineDistance(tRef, tObj);
      if (tError < minError) {
        minError = tError;
        tObj->match = tRef;
        tObj->error = tError;
      }
      tRef = tRef->next;
    }
    tObj = tObj->next;
  }

#ifdef PRINT_CM_DETAIL
  printf("no partition match done!\n");
#endif
}

void CrossMatchSphere::freeStarList(CMStar *starList) {

  if (NULL != starList) {
    CMStar *tStar = starList->next;
    while (tStar) {
      starList->next = tStar->next;
      free(tStar->line);
      free(tStar);
      tStar = starList->next;
    }
    free(starList);
  }
}

void CrossMatchSphere::freeAllMemory() {
  
  //释放在PartitionSphere::getMatchStar3匹配中，复制出来的CMStar
  //该匹配方式记录所有匹配的星，需要对每个匹配的星做一个拷贝
  if (NULL != objStarFile){
    CMStar *starList = objStarFile->starList;
    while (starList) {
      freeStarList(starList->match);
      starList = starList->next;
    }
  }
  if (NULL != refStarFile)
    refStarFile->~StarFile();
  if (NULL != objStarFile)
    objStarFile->~StarFile();
  if (NULL != refStarFileNoPtn)
    refStarFileNoPtn->~StarFile();
  if (NULL != objStarFileNoPtn)
    objStarFileNoPtn->~StarFile();
  if (NULL != zones) {
    zones->freeZoneArray();
  }
}

void CrossMatchSphere::compareResult(char *refName, char *objName, char *outfName, float errorBox) {

  match(refName, objName, errorBox);
  matchNoPartition(refName, objName, errorBox); 

  FILE *fp = fopen(outfName, "w");

  CMStar *tStar1 = objStarFile->starList;
  CMStar *tStar2 = objStarFileNoPtn->starList;
  int i = 0, j = 0, k = 0, m = 0, n = 0, g = 0;
  while (NULL != tStar1 && NULL != tStar2) {
    if (NULL != tStar1->match && NULL != tStar2->match) {
      i++;
      float errDiff = fabs(tStar1->error - tStar2->error);
      if (errDiff < CompareFloat)
        n++;
    } else if (NULL != tStar1->match && NULL == tStar2->match) {
      j++;
    } else if (NULL == tStar1->match && NULL != tStar2->match) {//ommit and OT
      k++;
      if (tStar2->error < errorBox)
        g++;
    } else {
      m++;
    }
    tStar1 = tStar1->next;
    tStar2 = tStar2->next;
  }
  fprintf(fp, "total star %d\n", i + j + k + m);
  fprintf(fp, "matched %d , two method same %d\n", i, n);
  fprintf(fp, "partition matched but nopartition notmatched %d\n", j);
  fprintf(fp, "nopartition matched but partition notmatched %d, small than errorBox %d\n", k, g);
  fprintf(fp, "two method are not matched %d\n", m);

  fprintf(fp, "\nX1,Y1,X1m,Y1m,err1 is the partition related info\n");
  fprintf(fp, "X2,Y2,X2m,Y2m,err2 is the nopartition related info\n");
  fprintf(fp, "X1,Y1,X2,Y2 is orig X and Y position of stars\n");
  fprintf(fp, "X1m,Y1m,X2m,Y2m is matched X and Y position of stars\n");
  fprintf(fp, "pos1,pos2 is the two method's match distance\n");
  fprintf(fp, "the following list is leaked star of partition method\n");
  fprintf(fp, "X1\tY1\tX2\tY2\tX1m\tY1m\tX2m\tY2m\tpos1\tpos2\n");
  tStar1 = objStarFile->starList;
  tStar2 = objStarFileNoPtn->starList;
  while (NULL != tStar1 && NULL != tStar2) {
    if (NULL == tStar1->match && NULL != tStar2->match && tStar2->error < errorBox) { //ommit and OT
      fprintf(fp, "%12f %12f %12f %12f %12f %12f %12f %12f %12f %12f\n",
              tStar1->pixx, tStar1->pixy, tStar2->pixx, tStar2->pixy,
              0.0, 0.0, tStar2->match->pixx, tStar2->match->pixy,
              tStar1->error, tStar2->error);
    }
    tStar1 = tStar1->next;
    tStar2 = tStar2->next;
  }

  fprintf(fp, "the following list is OT\n");
  fprintf(fp, "X1\tY1\tX2\tY2\tX1m\tY1m\tX2m\tY2m\tpos1\tpos2\n");
  tStar1 = objStarFile->starList;
  tStar2 = objStarFileNoPtn->starList;
  while (NULL != tStar1 && NULL != tStar2) {
    if (NULL == tStar1->match && NULL != tStar2->match && tStar2->error > errorBox) { //ommit and OT
      fprintf(fp, "%12f %12f %12f %12f %12f %12f %12f %12f %12f %12f\n",
              tStar1->pixx, tStar1->pixy, tStar2->pixx, tStar2->pixy,
              0.0, 0.0, tStar2->match->pixx, tStar2->match->pixy,
              tStar1->error, tStar2->error);
    }
    tStar1 = tStar1->next;
    tStar2 = tStar2->next;
  }

  fclose(fp);
  freeAllMemory();
}

void CrossMatchSphere::printMatchedRst(char *outfName, float errorBox) {

  FILE *fp = fopen(outfName, "w");
  fprintf(fp, "Id\tX\tY\tmId\tmX\tmY\tdistance\n");

  long count = 0;
  CMStar *tStar = objStarFile->starList;
  while (NULL != tStar) {
    if (NULL != tStar->match && tStar->error < errorBox) {
      fprintf(fp, "%8ld %12f %12f %8ld %12f %12f %12f\n",
              tStar->starId, tStar->pixx, tStar->pixy, tStar->match->starId,
              tStar->match->pixx, tStar->match->pixy, tStar->error);
      count++;
    }
    tStar = tStar->next;
  }
  fclose(fp);

#ifdef PRINT_CM_DETAIL
  printf("matched stars %d\n", count);
#endif
}

void CrossMatchSphere::printMatchedRst(char *outfName, CMStar *starList, float errorBox) {

  FILE *fp = fopen(outfName, "w");
  fprintf(fp, "Id\tX\tY\tmId\tmX\tmY\tdistance\n");

  long count = 0;
  CMStar *tStar = starList;
  while (NULL != tStar) {
    if (NULL != tStar->match && tStar->error < errorBox) {
      fprintf(fp, "%8ld %12f %12f %8ld %12f %12f %12f\n",
              tStar->starId, tStar->pixx, tStar->pixy, tStar->match->starId,
              tStar->match->pixx, tStar->match->pixy, tStar->error);
      count++;
    }
    tStar = tStar->next;
  }
  fclose(fp);

#ifdef PRINT_CM_DETAIL
  printf("matched stars %d\n", count);
#endif
}

void CrossMatchSphere::printAllStarList(char *outfName, CMStar *starList, float errorBox) {

  FILE *fp = fopen(outfName, "w");
  fprintf(fp, "Id\tX\tY\tmId\tmX\tmY\tdistance\n");

  long count = 0;
  CMStar *tStar = starList;
  while (NULL != tStar) {
    if (NULL != tStar->match) {
      fprintf(fp, "%8ld %12f %12f %8ld %12f %12f %12f\n",
              tStar->starId, tStar->pixx, tStar->pixy, tStar->match->starId,
              tStar->match->pixx, tStar->match->pixy, tStar->error);
    } else {
      fprintf(fp, "%8ld %12f %12f %8d %12f %12f %12f\n",
              tStar->starId, tStar->pixx, tStar->pixy, 0, 0.0, 0.0, tStar->error);
    }
    count++;
    tStar = tStar->next;
  }
  fclose(fp);

#ifdef PRINT_CM_DETAIL
  printf("matched stars %d\n", count);
#endif
}

void CrossMatchSphere::printOTStar(char *outfName, float errorBox) {

  FILE *fp = fopen(outfName, "w");
  fprintf(fp, "Id\tra\tdec\tmId\tmra\tmdec\tdistance\n");

  long count = 0;
  CMStar *tStar = objStarFile->starList;
  while (NULL != tStar) {
    if (NULL == tStar->match || tStar->error > errorBox) {
      fprintf(fp, "%8ld %12f %12f ",
              tStar->starId, tStar->alpha, tStar->delta);
      if (NULL != tStar->match) {
        fprintf(fp, "%8ld %12f %12f %12f",
                tStar->match->starId, tStar->match->alpha, tStar->match->delta, tStar->error);
      }
      fprintf(fp, "\n");
      count++;
    }
    tStar = tStar->next;
  }
  fclose(fp);

#ifdef PRINT_CM_DETAIL
  printf("OT stars %d\n", count);
#endif
}

void CrossMatchSphere::printOTStar2(char *outfName, float errorBox) {

  FILE *fp = fopen(outfName, "w");

  long count = 0;
  CMStar *tStar = objStarFile->starList;
  int t0 = 0, t1 = 0, t2 = 0;
  while (NULL != tStar) {
    if (tStar->matchNum < 1) {
      printf("%5d %s", tStar->id, tStar->line);
      t0++;
    } else if (tStar->matchNum == 1) {
      t1++;
    } else {
      t2++;
    }
    if (NULL == tStar->match && tStar->matchNum <= 1) {
      fprintf(fp, "%s", tStar->line);
      count++;
    }
    tStar = tStar->next;
  }
  fclose(fp);

#ifdef PRINT_CM_DETAIL
  //  printf("t0=%d\n", t0);
  //  printf("t1=%d\n", t1);
  //  printf("t2=%d\n", t2);
#endif
  printf("OT stars %d\n", count);
}