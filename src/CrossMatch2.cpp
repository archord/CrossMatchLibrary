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
#include "CrossMatch2.h"

CrossMatch::CrossMatch() {

  refNum = 0;
  objNum = 0;
  refStarList = NULL;
  objStarList = NULL;
  refStarListNoPtn = NULL;
  objStarListNoPtn = NULL;
  zones = NULL;
  sphereZones = NULL;
}

CrossMatch::CrossMatch(const CrossMatch& orig) {
}

CrossMatch::~CrossMatch() {
}

void CrossMatch::matchSphere(char *refName, char *ref2Name, char *objName, float errorBox, float errorBox2) {

  errorBox = errorBox / 3600.0;
  errorBox2 = errorBox2 / 3600.0;

  float minZoneLen = errorBox * TimesOfErrorRadius;
  float searchRds = errorBox;
  float minZoneLen2 = errorBox2 * TimesOfErrorRadius;
  float searchRds2 = errorBox2;

  refStarList = readStarFileSphere(refName, refNum);
  refStarList2 = readStarFileSphere(ref2Name, refNum);
  objStarList = readStarFileSphere(objName, objNum);

  sphereZones = new PartitionSphere(errorBox, minZoneLen, searchRds);
  sphereZones->initAreaNode(refStarList);
  sphereZones->addDataToTree(refStarList);
  refStarList = NULL;

  int i = 0;
  CMStar *nextStar = objStarList;
  while (nextStar) {
    sphereZones->getMatchStar(nextStar);
    nextStar = nextStar->next;
    i++;
  }
  //  printf("i=%d\n", i);
  sphereZones2 = new PartitionSphere(errorBox2, minZoneLen2, searchRds2);
  sphereZones2->initAreaNode(refStarList2);
  sphereZones2->addDataToTree(refStarList2);
  refStarList2 = NULL;
  nextStar = objStarList;
  i = 0;
  while (nextStar) {
    i++;
    sphereZones2->getMatchStar2(nextStar);
    nextStar = nextStar->next;
  }
  //  printf("i=%d\n", i);

#ifdef PRINT_CM_DETAIL
  printf("partition match done!\n");
#endif
}

void CrossMatch::match(char *refName, char *objName, float errorBox) {

  float minZoneLen = errorBox * TimesOfErrorRadius;
  float searchRds = errorBox;

  refStarList = readStarFile(refName, refNum);
  objStarList = readStarFile(objName, objNum);

  zones = new Partition(errorBox, minZoneLen, searchRds);
  zones->partitonStarField(refStarList, refNum);
  refStarList = NULL;

  CMStar *nextStar = objStarList;
  while (nextStar) {
    zones->getMatchStar(nextStar);
    nextStar = nextStar->next;
  }

#ifdef PRINT_CM_DETAIL
  printf("partition match done!\n");
#endif
}

void CrossMatch::match(CMStar *ref, int refNum, CMStar *obj, float errorBox) {

  float minZoneLen = errorBox * TimesOfErrorRadius;
  float searchRds = errorBox;

  zones = new Partition(errorBox, minZoneLen, searchRds);
  zones->partitonStarField(ref, refNum);
  ref = NULL;

  CMStar *nextStar = obj;
  while (nextStar) {
    zones->getMatchStar(nextStar);
    nextStar = nextStar->next;
  }

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
void CrossMatch::matchNoPartition(char *refName, char *objName, float errorBox) {

  int refNum, objNum;
  refStarListNoPtn = readStarFile(refName, refNum);
  objStarListNoPtn = readStarFile(objName, objNum);

  CMStar *tObj = objStarListNoPtn;

  while (tObj) {
    float tError = 0.0;
    float minError = getLineDistance(refStarListNoPtn, tObj);
    tObj->match = refStarListNoPtn;
    tObj->error = minError;
    CMStar *tRef = refStarListNoPtn->next;
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

  //freeStarList(refStarListNoPtn);
  //freeStarList(objStarListNoPtn);
#ifdef PRINT_CM_DETAIL
  printf("no partition match done!\n");
#endif
}

void CrossMatch::matchNoPartition2(char *refName, char *objName, float errorBox) {

  int refNum, objNum;
  refStarListNoPtn = readStarFile(refName, refNum);
  objStarListNoPtn = readStarFile(objName, objNum);

  CMStar *tObj = objStarListNoPtn;

  while (tObj) {
    float tError = 0.0;
    float minError = getGreatCircleDistance(refStarListNoPtn, tObj);
    tObj->match = refStarListNoPtn;
    tObj->error = minError;
    CMStar *tRef = refStarListNoPtn->next;
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

  //freeStarList(refStarListNoPtn);
  //freeStarList(objStarListNoPtn);
#ifdef PRINT_CM_DETAIL
  printf("no partition match done!\n");
#endif
}

/**
 * the matched star is stored on obj->match, 
 * the distance between two stars is stored on obj->error
 * @param ref
 * @param obj
 */
void CrossMatch::matchNoPartition(CMStar *ref, CMStar *obj, float errorBox) {

  CMStar *tObj = obj;

  while (tObj) {
    float tError = 0.0;
    float minError = getLineDistance(ref, tObj);
    tObj->match = refStarListNoPtn;
    tObj->error = minError;
    CMStar *tRef = ref->next;
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
}

/**
 * default only read first three column(X, Y, mag)
 * @param fName
 * @param starNum
 * @return 
 */
CMStar *CrossMatch::readStarFile(char *fName, int &starNum) {

  FILE *fp = fopen(fName, "r");
  if (fp == NULL) {
    return NULL;
  }

  starNum = 0;
  CMStar *starList = NULL;
  CMStar *tStar = NULL;
  CMStar *nextStar = NULL;
  char line[MaxStringLength];

  float x, y, mag;

  while (fgets(line, MaxStringLength, fp) != NULL) {
    if (3 == sscanf(line, "%f%f%f%*s", &x, &y, &mag)) {
      nextStar = (CMStar *) malloc(sizeof (CMStar));
      nextStar->id = starNum;
      nextStar->pixx = x;
      nextStar->pixy = y;
      nextStar->mag = mag;
      nextStar->line = (char *) malloc(MaxStringLength);
      strcpy(nextStar->line, line);
      nextStar->next = NULL;
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
  printf("%s\t%d stars\n", fName, starNum);
#endif
  return starList;
}

CMStar *CrossMatch::readStarFileSphere(char *fName, int &starNum) {

  FILE *fp = fopen(fName, "r");
  if (fp == NULL) {
    return NULL;
  }

  starNum = 0;
  CMStar *starList = NULL;
  CMStar *tStar = NULL;
  CMStar *nextStar = NULL;
  char line[MaxStringLength];
  float ra, dec;

  while (fgets(line, MaxStringLength, fp) != NULL) {
    if (2 == sscanf(line, "%f%f*s", &ra, &dec)) {
      nextStar = (CMStar *) malloc(sizeof (CMStar));
      nextStar->id = starNum;
      nextStar->alpha = ra;
      nextStar->delta = dec;
      nextStar->line = (char *) malloc(MaxStringLength);
      strcpy(nextStar->line, line);
      nextStar->matchNum = 0;
      nextStar->next = NULL;
      nextStar->match = NULL;
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
  printf("%s\t%d stars\n", fName, starNum);
#endif
  return starList;
}

CMStar *CrossMatch::copyStarList(CMStar *starlst) {

  CMStar *tHead = NULL;
  CMStar *tStar = NULL;
  CMStar *origlst = starlst;

  if (NULL != origlst) {
    tHead = (CMStar *) malloc(sizeof (CMStar));
    memcpy(tHead, origlst, sizeof (CMStar));
    origlst = origlst->next;
    tStar = tHead;
  }

  while (NULL != origlst) {
    tStar->next = (CMStar *) malloc(sizeof (CMStar));
    memcpy(tStar->next, origlst, sizeof (CMStar));
    origlst = origlst->next;
    tStar = tStar->next;
  }
  return tHead;
}

void CrossMatch::freeStarList(CMStar *starList) {

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

void CrossMatch::freeAllMemory() {

  freeStarList(refStarList);
  freeStarList(objStarList);
  freeStarList(refStarListNoPtn);
  freeStarList(objStarListNoPtn);
  if (NULL != zones) {
    zones->freeZoneArray();
  }
}

void CrossMatch::compareResult(char *refName, char *objName, char *outfName, float errorBox) {

  match(refName, objName, errorBox);
  matchNoPartition(refName, objName, errorBox);

  FILE *fp = fopen(outfName, "w");

  CMStar *tStar1 = objStarList;
  CMStar *tStar2 = objStarListNoPtn;
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
  tStar1 = objStarList;
  tStar2 = objStarListNoPtn;
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
  tStar1 = objStarList;
  tStar2 = objStarListNoPtn;
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

void CrossMatch::printMatchedRst(char *outfName, float errorBox) {

  FILE *fp = fopen(outfName, "w");
  fprintf(fp, "Id\tX\tY\tmId\tmX\tmY\tdistance\n");

  long count = 0;
  CMStar *tStar = objStarList;
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

void CrossMatch::printMatchedRstSphere(char *outfName, float errorBox) {

  FILE *fp = fopen(outfName, "w");
  fprintf(fp, "Id\tra\tdec\tmId\tmra\tmdec\tdistance\n");

  long count = 0;
  CMStar *tStar = objStarList;
  while (NULL != tStar) {
    if (NULL != tStar->match && tStar->error < errorBox) {
      fprintf(fp, "%8ld %12f %12f %8ld %12f %12f %12f\n",
              tStar->starId, tStar->alpha, tStar->delta, tStar->match->starId,
              tStar->match->alpha, tStar->match->delta, tStar->error);
      count++;
    }
    tStar = tStar->next;
  }
  fclose(fp);

#ifdef PRINT_CM_DETAIL
  printf("matched stars %d\n", count);
#endif
}

void CrossMatch::printMatchedRst(char *outfName, CMStar *starList, float errorBox) {

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

void CrossMatch::printAllStarList(char *outfName, CMStar *starList, float errorBox) {

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

void CrossMatch::printOTStar(char *outfName, float errorBox) {

  FILE *fp = fopen(outfName, "w");
  fprintf(fp, "Id\tX\tY\n");

  long count = 0;
  CMStar *tStar = objStarList;
  while (NULL != tStar) {
    if (NULL == tStar->match) {
      fprintf(fp, "%8ld %12f %12f\n",
              tStar->starId, tStar->pixx, tStar->pixy);
      count++;
    }
    tStar = tStar->next;
  }
  fclose(fp);

#ifdef PRINT_CM_DETAIL
  printf("OT stars %d\n", count);
#endif
}

void CrossMatch::printOTStarSphere(char *outfName, float errorBox) {

  FILE *fp = fopen(outfName, "w");
  fprintf(fp, "Id\tra\tdec\tmId\tmra\tmdec\tdistance\n");

  long count = 0;
  CMStar *tStar = objStarList;
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

void CrossMatch::printOTStarSphere2(char *outfName, float errorBox) {

  FILE *fp = fopen(outfName, "w");

  long count = 0;
  CMStar *tStar = objStarList;
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