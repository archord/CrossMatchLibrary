/* 
 * File:   CrossMatch.h
 * Author: xy
 *
 * Created on October 18, 2013, 8:48 AM
 */

#ifndef CROSSMATCH_H
#define	CROSSMATCH_H

#include "Partition.h"
#include "PartitionSphere.h"

class CrossMatch {
public:

    static const int TimesOfErrorRadius = 10; //length of search radius

    CrossMatch();
    CrossMatch(const CrossMatch& orig);
    virtual ~CrossMatch();

    CMStar *readStarFile(char *fName, int &starNum);
    CMStar *readStarFileSphere(char *fName, int &starNum);
    CMStar *copyStarList(CMStar *starlst);
    void match(char *refName, char *objName, float errorBox);
    void matchSphere(char *refName, char *ref2Name, char *objName, float errorBox, float errorBox2);
    void match(CMStar *ref, int refNum, CMStar *obj, float errorBox);
    void compareResult(char *refName, char *objName, char *outName, float errorBox);
    void matchNoPartition(char *refName, char *objName, float errorBox);
    void matchNoPartition2(char *refName, char *objName, float errorBox);
    void matchNoPartition(CMStar *ref, CMStar *obj, float errorBox);
    void printMatchedRst(char *outfName, float errorBox);
    void printMatchedRstSphere(char *outfName, float errorBox);
    void printMatchedRst(char *outfName, CMStar *starList, float errorBox);
    void printOTStar(char *outfName, float errorBox);
    void printOTStarSphere(char *outfName, float errorBox);
    void printOTStarSphere2(char *outfName, float errorBox);
    void printAllStarList(char *outfName, CMStar *starList, float errorBox);
    void freeStarList(CMStar *starList);
    void freeAllMemory();
    
protected:
    
    int refNum;
    int refNum2;
    int objNum;
    CMStar *refStarList;
    CMStar *refStarList2;
    CMStar *objStarList;
    CMStar *refStarListNoPtn;
    CMStar *objStarListNoPtn;
    
protected:
    float errRadius;            //两颗星匹配的最小距离
    Partition *zones;
    PartitionSphere *sphereZones;
    PartitionSphere *sphereZones2;
};

#endif	/* CROSSMATCH_H */

