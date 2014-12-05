

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "CrossMatch.h"

void testCrossMatch() {
    
    char *refName = "data/referance.cat";
    char *objName = "data/object.cat";
    char *matchedName = "data/matched.cat";
    char *otName = "data/ot.cat";
    float errorBox = 0.7;
    
    CrossMatch *cm = new CrossMatch();
    cm->match(refName, objName, errorBox);
    cm->printMatchedRst(matchedName, errorBox);
    cm->printOTStar(otName, errorBox);
    cm->freeAllMemory();

}

void partitionAndNoPartitionCompare() {
    
    char *refName = "data/referance.cat";
    char *objName = "data/object.cat";
    char *cmpOutName = "data/cmpOut.cat";
    float errorBox = 0.7;
    
    CrossMatch *cm = new CrossMatch();
    cm->compareResult(refName, objName, cmpOutName, errorBox);
    cm->freeAllMemory();

}
