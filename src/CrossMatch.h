/* 
 * File:   CrossMatch.h
 * Author: xy
 *
 * Created on October 18, 2013, 8:48 AM
 */

#ifndef CROSSMATCH_H
#define	CROSSMATCH_H

#include "StarFile.h"
#include "Partition.h"
#include "PartitionSphere.h"

class CrossMatch {
public:

  static const int TimesOfErrorRadius = 10; //length of search radius

  CrossMatch();
  CrossMatch(const CrossMatch& orig);
  virtual ~CrossMatch();

  void match(char *refName, char *objName, float errorBox);
  void match(StarFile *ref, StarFile *obj, float errorBox);
  void match(char *refName, char *objName, float errorBox, int *idxs);
  
  void compareResult(char *refName, char *objName, char *outName, float errorBox);
  void matchNoPartition(char *refName, char *objName, float errorBox);
  void matchNoPartition(StarFile *ref, StarFile *obj, float errorBox);
  void printMatchedPairs(char *outfName, float errorBox);
  void printMatchedPairs(char *outfName, StarFile *starList, float errorBox);
  void printOTStar(char *outfName, float errorBox);
  void printAllStarList(char *outfName, StarFile *starList, float errorBox);
  void freeAllMemory();
  void testCrossMatch();
  void partitionAndNoPartitionCompare();
  
  void printMatched(char *outfName, CMStar *tstars, float errorBox);
  void printNotMatched(char *outfName, CMStar *tstars);
  void printMatchedDs9(char *outfName, CMStar *tstars, float errorBox);
  void printNotMatchedDs9(char *outfName, CMStar *tstars);

  StarFile *refStarFile;
  StarFile *objStarFile;
  StarFile *refStarFileNoPtn;
  StarFile *objStarFileNoPtn;

protected:
  float errRadius; //两颗星匹配的最小距离
  Partition *zones;
};


#endif	/* CROSSMATCH_H */

