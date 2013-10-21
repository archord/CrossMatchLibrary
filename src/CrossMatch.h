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

    static const long MaxMallocMemory = 2147483648l; //1GB=1073741824l 2GB=2147483648l
    static const int TimesOfErrorRadius = 10; //length of search radius
    static const int MaxStringLength = 1024;
    static const int DiffStarDistance = 3;
    static const float CompareFloat = 0.000001;

    CrossMatch();
    CrossMatch(const CrossMatch& orig);
    virtual ~CrossMatch();

    cm_star *readStarFile(char *fName, int &starNum);
    void match(char *refName, char *objName, float errorBox);
    void compareResult(char *refName, char *objName, char *outName, float errorBox);
    void matchNoPartition(char *refName, char *objName, float errorBox);
    void matchNoPartition(cm_star *ref, cm_star *obj, float errorBox);
    void printMatchedRst(char *outfName, float errorBox);
    void printOTStar(char *outfName, float errorBox);
    void freeStarList(cm_star *starList);
    void freeAllMemory();
private:
    float errRadius;            //两颗星匹配的最小距离
    cm_star *refStarList;
    cm_star *objStarList;
    cm_star *refStarListNoPtn;
    cm_star *objStarListNoPtn;
    Partition *zones;
};

#endif	/* CROSSMATCH_H */

