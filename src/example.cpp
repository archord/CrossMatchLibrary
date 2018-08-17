

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "CrossMatch.h"

//testCrossMatch

int main(int argc, char **argv) {

  if (!(argc == 5 || argc ==9)) {
    printf("Usage: crossmatch ref.cat obj.cat out.cat errbox [paramNum] [xidx] [yidx] [magidx] \n");
    return 0;
  }

  char *refName = argv[1];
  char *objName = argv[2];
  
  char *otName = argv[3];
  float errorBox1 = atof(argv[4]);
  
  int pNum = atoi(argv[5]);
  int idxs = {atoi(argv[6]),atoi(argv[7]),atoi(argv[8])};

  CrossMatch *cm = new CrossMatch();
  cm->match(refName, objName, errorBox1);
  cm->printMatchedRst(otName, errorBox1);
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
