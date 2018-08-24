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
#include "CrossMatchSelf.h"
#include "cmhead.h"
#include "StarFile.h"

using namespace std;


PartitionSelf::PartitionSelf() {
  starCount = 0;
  rptStar = 0;
}

PartitionSelf::PartitionSelf(float errBox, float minZoneLen, float searchRds) {
  starCount = 0;
  rptStar = 0;
  errRadius = errBox;
  minZoneLength = minZoneLen;
  searchRadius = searchRds;
}

/**
 * 
 * @param zoneIdx
 * @param objStar
 */
void PartitionSelf::searchSimilarStar(long zoneIdx, CMStar *objStar) {

  float error = errRadius;
  //CMStar *matchStar = objStar->match;
  CMStar *nextStar = zoneArray[zoneIdx].star;
  CMStar *topStar = zoneArray[zoneIdx].star;
  CMStar *tmpStar = NULL;

  /*
  while (NULL != matchStar && NULL != matchStar->match) {
    matchStar = matchStar->match;
  }
   */

  /**
   * 如果区域中的第一个星就匹配，则在去掉这个星后，区域的头指针需要指向下一个星
   * 在区域中去掉一颗星后，区域的星的总数要减1
   */
  while (nextStar) {
    float distance = getLineDistance(nextStar, objStar);
    if (distance < error) {
      if(distance<objStar->error){
        objStar->error = distance;
      }
      nextStar->error = distance;
#ifdef PRINT_CM_DETAILa
      printf("mtch star id: %d\n", nextStar->id);
#endif
      tmpStar = nextStar;
      if (zoneArray[zoneIdx].star == nextStar) {
        zoneArray[zoneIdx].star = nextStar->next;
        topStar = zoneArray[zoneIdx].star;
        nextStar = zoneArray[zoneIdx].star;
      } else {
        topStar->next = nextStar->next;
        nextStar = nextStar->next;
      }
      
      tmpStar->next = NULL;
      zoneArray[zoneIdx].starNum--;
      starCount++;
      
      tmpStar->error = distance;
      tmpStar->match=objStar->match;
      objStar->match = tmpStar;
      objStar->matchNum ++;
      
    } else {
      topStar = nextStar;
      nextStar = nextStar->next;
    }

  }
}

CMStar *PartitionSelf::match() {

  CMStar *headStar = NULL;
  CMStar *curStar = NULL;
  int starClassify = 0;

  for (int idx = 0; idx < totalZone; idx++) {
    if (zoneArray[idx].starNum > 0) {
#ifdef PRINT_CM_DETAILa
      printf("**************\n");
      printf("zone id: %d\n", idx);
      printf("zone star: %d\n", zoneArray[idx].starNum);
#endif
      while (NULL != zoneArray[idx].star) {
        //headStar指向结果列表的第一颗星
        //curStar指向当前待匹配的星
        if (NULL == headStar) {
          headStar = zoneArray[idx].star;
          curStar = headStar;
        } else {
          curStar->next = zoneArray[idx].star;
          curStar = curStar->next;
        }
#ifdef PRINT_CM_DETAILa
        printf("obj star id: %ld\n", curStar->id);
#endif
        //去掉当前这颗星
        zoneArray[idx].star = curStar->next;
        curStar->next = NULL;
        curStar->match = NULL;
        //分区中星的总数减一
        zoneArray[idx].starNum--;
        starCount++;
        //分类星加一
        starClassify++;
        //寻找分类星的匹配星
        long sZoneNum = 0;
        long *searchZonesIdx = getStarSearchZone(curStar, sZoneNum);
        for (int i = 0; i < sZoneNum; i++) {
          searchSimilarStar(searchZonesIdx[i], curStar);
        }
        free(searchZonesIdx);
      }
    }
  }

#ifdef PRINT_CM_DETAIL
  printf("single stars: %d\n", starClassify);
  printf("total records: %ld\n", starCount);
  printf("total repeat records: %ld\n", rptStar);
#endif

  return headStar;
}

void CrossMatchSelf::match(char *infile, float errorBox, int *idxs) {

  float minZoneLen = errorBox * TimesOfErrorRadius;
  float searchRds = errorBox;

  StarFile *tstarFile = new StarFile();
  tstarFile->readStar(infile, idxs);

  zones = new PartitionSelf(errorBox, minZoneLen, searchRds);
  zones->partitonStarField(tstarFile);
  tstarFile->starList = NULL;
  rstStar = zones->match();

#ifdef PRINT_CM_DETAIL
  printf("check match result:\n");  
  int tnum = 0;
  CMStar *tStar = rstStar;
  while (tStar) {
    tnum = tStar->matchNum + tnum + 1;
    tStar = tStar->next;
  }
  printf("total init star: %ld, result star: %d\n", tstarFile->starNum, tnum);
#endif

#ifdef PRINT_CM_DETAIL
  printf("match done!\n");
#endif
}


void CrossMatchSelf::freeAllMemory() {
  freeRstList(rstStar);
  zones->freeZoneArray();
}

void CrossMatchSelf::freeRstList(CMStar *starList) {

  CMStar *tStar = starList;
  while (NULL != tStar) {
    freeMchList(tStar->match);
    tStar = tStar->next;
  }
  freeStarList(starList);
}

void CrossMatchSelf::freeStarList(CMStar *starList) {

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

void CrossMatchSelf::freeMchList(CMStar *starList) {

  if (NULL != starList) {
    CMStar *tStar = starList->match;
    while (tStar) {
      starList->match = tStar->match;
      free(((CMStar *) tStar)->line);
      free(tStar);
      tStar = starList->match;
    }
    free(starList);
  }
}


