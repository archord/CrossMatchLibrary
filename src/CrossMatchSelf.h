/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CrossMatchSelf.h
 * Author: xy
 *
 * Created on August 17, 2018, 4:38 PM
 */

#ifndef CROSSMATCHSELF_H
#define CROSSMATCHSELF_H

#include "Partition.h"
#include "CrossMatch.h"

//#define PRINT_CM_DETAIL

class PartitionSelf : public Partition {
public:
  long starCount;
  long rptStar;
public:
  PartitionSelf();
  PartitionSelf(float errBox, float minZoneLen, float searchRds);
  void searchSimilarStar(long zoneIdx, CMStar *objStar);
  CMStar *match();
  bool hasSame(CMStar *mchStar, CMStar *star);
  void addMchSort(CMStar *mchStar, CMStar *star);
  void swapValue(CMStar *star1, CMStar *star2);
};

class CrossMatchSelf : public CrossMatch {
public:
  CMStar *rstStar;
  PartitionSelf *zones;

public:
  void match(char *infile, float errorBox, int *idxs = NULL);
  void freeAllMemory();
  void freeStarList(CMStar *starList);
  void freeMchList(CMStar *starList);
  void freeRstList(CMStar *starList);
};

#endif /* CROSSMATCHSELF_H */

