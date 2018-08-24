
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "CrossMatch.h"
#include "CrossMatchSelf.h"

using namespace std;

void testCrossMatchMain(int argc, char **argv);
void testSelfCrossMatchMain(int argc, char **argv);
void partitionAndNoPartitionCompare();
void testSelfCrossMatch(char *objName, float errorBox, int *idxs);

int main(int argc, char **argv) {

  if (argc == 4 || argc == 8) {
    testCrossMatchMain(argc, argv);
  } else if (argc == 6 || argc == 3) {
    testSelfCrossMatchMain(argc, argv);
  }
  return 0;
}

void testSelfCrossMatchMain(int argc, char **argv) {

  if (!(argc == 6 || argc == 3)) {
    printf("Usage: selfmatch obj.cat errbox [xidx] [yidx] [magidx] \n");
    return;
  }

  char *objName = argv[1];
  float errorBox = atof(argv[2]);
  int idxs[] = {atoi(argv[3]), atoi(argv[4]), atoi(argv[5])};

  testSelfCrossMatch(objName, errorBox, idxs);
}

void testSelfCrossMatch(char *objName, float errorBox, int *idxs) {

  //char *objName = "data/object.cat";
  char outName1[1024];
  char outName2[1024];
  char outName3[1024];
  char outName4[1024];
  string tname(objName);
  string preStr = tname.substr(0, tname.find('.'));
  const char *preChar = preStr.c_str();
  sprintf(outName1, "%s_matched.cat", preChar);
  sprintf(outName2, "%s_notMatched.cat", preChar);
  sprintf(outName3, "%s_matchedDs9.reg", preChar);
  sprintf(outName4, "%s_notMatchedDs9.reg", preChar);

  //  float errorBox = 32;

  //  int idxs[] = {1,2,7};
  //  int pnum = 10;
  CrossMatchSelf *cm = new CrossMatchSelf();
  cm->match(objName, errorBox, idxs);
  cm->printMatched(outName1, cm->rstStar, errorBox);
  cm->printNotMatched(outName2, cm->rstStar);
  cm->printMatchedDs9(outName3, cm->rstStar, errorBox);
  cm->printNotMatchedDs9(outName4, cm->rstStar);

  cm->freeAllMemory();
}

void testCrossMatchMain(int argc, char **argv) {
  if (!(argc == 4 || argc == 8)) {
    printf("Usage: crossmatch ref.cat obj.cat out.cat errbox [xidx] [yidx] [magidx] \n");
    return;
  }

  char *refName = argv[1];
  char *objName = argv[2];

  char *outName = argv[3];
  float errorBox = atof(argv[4]);

  int idxs[] = {atoi(argv[5]), atoi(argv[6]), atoi(argv[7])};

  char outName1[1024];
  char outName2[1024];
  char outName3[1024];
  char outName4[1024];
  string tname(objName);
  string preStr = tname.substr(0, tname.find('.'));
  const char *preChar = preStr.c_str();
  sprintf(outName1, "%s_matched.cat", preChar);
  sprintf(outName2, "%s_notMatched.cat", preChar);
  sprintf(outName3, "%s_matchedDs9.reg", preChar);
  sprintf(outName4, "%s_notMatchedDs9.reg", preChar);

  CrossMatch *cm = new CrossMatch();
  cm->match(refName, objName, errorBox, idxs);
  cm->printMatched(outName1, cm->objStarFile->starList, errorBox);
  cm->printNotMatched(outName2, cm->objStarFile->starList);
  cm->printMatchedDs9(outName3, cm->objStarFile->starList, errorBox);
  cm->printNotMatchedDs9(outName4, cm->objStarFile->starList);

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
