

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "CrossMatch.h"

//testCrossMatch

int main(int argc, char **argv) {

  if (argc != 7) {
    printf("Usage: crossmatch ref1.cat ref2.cat obj.cat out.cat errbox1 errbox2\n");
    return 0;
  }

  char *refName = argv[1];
  char *ref2Name = argv[2];
  char *objName = argv[3];
//  char *matchedName = "data/matched.cat";
  char *otName = argv[4];
  float errorBox1 = atof(argv[5]);
  float errorBox2 = atof(argv[6]);

  CrossMatch *cm = new CrossMatch();
//  cm->matchSphere(refName, ref2Name, objName, errorBox1, errorBox2);
//  cm->printMatchedRstSphere(matchedName, errorBox1);
//  cm->printOTStarSphere2(otName, errorBox1);
  cm->freeAllMemory();
  return 0;
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
