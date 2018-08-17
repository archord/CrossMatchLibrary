

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "CrossMatch.h"
#include "CrossMatchSelf.h"

void testCrossMatch(int argc, char **argv);
void partitionAndNoPartitionCompare();
void testSelfCrossMatch();

int main(int argc, char **argv) {

  testSelfCrossMatch();
//  partitionAndNoPartitionCompare();
  return 0;
}

void testSelfCrossMatch(){
  
  char *objName = "data/object.cat";
  char *outName1 = "data/selfCM_matched.cat";
  char *outName2 = "data/selfCM_notMatched.cat";
  char *outName3 = "data/selfCM_matchedDs9.cat";
  char *outName4 = "data/selfCM_notMatchedDs9.cat";
  float errorBox = 32;
  
  int idxs[] = {1,2,7};
  int pnum = 10;
  CrossMatchSelf *cm = new CrossMatchSelf();
  cm->match(objName, errorBox, idxs, pnum);
  cm->printMatched(outName1, cm->rstStar, errorBox);
  cm->printNotMatched(outName2, cm->rstStar);
  cm->printMatchedDs9(outName3, cm->rstStar, errorBox);
  cm->printNotMatchedDs9(outName4, cm->rstStar);
  
  cm->freeAllMemory();
}

void testCrossMatch(int argc, char **argv) {
  if (!(argc == 5 || argc == 9)) {
    printf("Usage: crossmatch ref.cat obj.cat out.cat errbox [paramNum] [xidx] [yidx] [magidx] \n");
    return;
  }

  char *refName = argv[1];
  char *objName = argv[2];

  char *otName = argv[3];
  float errorBox1 = atof(argv[4]);

  int pNum = atoi(argv[5]);
  int idxs[] = {atoi(argv[6]), atoi(argv[7]), atoi(argv[8])};

  CrossMatch *cm = new CrossMatch();
  cm->match(refName, objName, errorBox1);
  cm->printMatchedPairs(otName, errorBox1);
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
