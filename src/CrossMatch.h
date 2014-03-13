/* 
 * File:   CrossMatch.h
 * Author: xy
 *
 * Created on October 18, 2013, 8:48 AM
 */

#ifndef CROSSMATCH_H
#define	CROSSMATCH_H

#include "Partition.h"

class CrossMatch {
public:

    static const int TimesOfErrorRadius = 10; //length of search radius

    CrossMatch();
    CrossMatch(const CrossMatch& orig);
    virtual ~CrossMatch();

    CMStar *readStarFile(char *fName, int &starNum);
    CMStar *copyStarList(CMStar *starlst);
    void match(char *refName, char *objName, float errorBox);
    void match(CMStar *ref, int refNum, CMStar *obj, float errorBox);
    void compareResult(char *refName, char *objName, char *outName, float errorBox);
    void matchNoPartition(char *refName, char *objName, float errorBox);
    void matchNoPartition(CMStar *ref, CMStar *obj, float errorBox);
    void printMatchedRst(char *outfName, float errorBox);
    void printMatchedRst(char *outfName, CMStar *starList, float errorBox);
    void printOTStar(char *outfName, float errorBox);
    void printAllStarList(char *outfName, CMStar *starList, float errorBox);
    void freeStarList(CMStar *starList);
    void freeAllMemory();
    
private:
    
    int refNum;
    int objNum;
    CMStar *refStarList;
    CMStar *objStarList;
    CMStar *refStarListNoPtn;
    CMStar *objStarListNoPtn;
    
protected:
    float errRadius;            //两颗星匹配的最小距离
    Partition *zones;
};

#endif	/* CROSSMATCH_H */

