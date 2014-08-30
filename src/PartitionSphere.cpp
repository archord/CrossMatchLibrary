/* 
 * File:   PartitionSphere.cpp
 * Author: xy
 * 
 * Created on October 18, 2013, 8:47 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "PartitionSphere.h"

PartitionSphere::PartitionSphere() {
}

PartitionSphere::PartitionSphere(const PartitionSphere& orig) {
}

PartitionSphere::PartitionSphere(float errBox, float minZoneLen, float searchRds) {
  areaBox = errBox;
  minZoneLength = minZoneLen;
  searchRadius = searchRds;

  areaWidth = 0;
  areaHeight = 0;
  zoneInterval = 0;
  planeZoneX = 0;
  planeZoneY = 0;
  showProcessInfo = 1;
}

PartitionSphere::~PartitionSphere() {

}

double PartitionSphere::angToRad(double angle) {
  return angle * ANG_TO_RAD;
}

float PartitionSphere::getAngleFromGreatCircle(double dec, double errorRadius) {
  double rst = acos((cos(errorRadius * ANG_TO_RAD) - pow(sin(dec * ANG_TO_RAD), 2)) / pow(cos(dec * ANG_TO_RAD), 2));
  return rst*RAD_TO_ANG;
}

long PartitionSphere::getPointBranch(CMStar *point) {

  float alpha = (point->alpha - raMini);
  float delta = (point->delta - decMini);
  int x = (int) (alpha * factor);
  int y = (int) (delta * factor);

  return y * raNode + x;
}

void PartitionSphere::addPointToBranchSort(CMStar *point, CMZone *branch) {

  point->next = NULL;
  CMStar *tmp = branch->star;
  if (tmp == NULL) {
    branch->star = point;
  } else {
    /**************************************/
    /*该部分是否考虑用二叉树或四叉树等其他优化查找的算法来实现*/
    /*当前按alpha的值从小到大排列*/
    /**/
    if (point->alpha < tmp->alpha) {
      branch->star = point;
      point->next = tmp;
    } else {
      CMStar *before = tmp;
      tmp = before->next;
      while ((tmp) && (point->alpha >= tmp->alpha)) { //当tmp的next为空时，tmp的下一个就是point的位置
        before = tmp;
        tmp = before->next;
      }
      before->next = point;
      point->next = tmp;
    }
  }
  branch->starNum = branch->starNum + 1;
}

void PartitionSphere::addPointToBranchNotSort(CMStar *point, CMZone *branch) {

  point->next = NULL;
  CMStar *tmp = branch->star;
  if (tmp == NULL) {
    branch->star = point;
  } else {
    CMStar *before = tmp;
    tmp = before->next;
    before->next = point;
    point->next = tmp;
  }
  branch->starNum = branch->starNum + 1;
}

void PartitionSphere::initRaRadiusIndex() {

  if (decMini > 0 && decMaxi > 0) {
    absDecMin = decMini;
    absDecMax = decMaxi;
  } else if (decMini < 0 && decMaxi < 0) {
    absDecMin = abs(decMaxi);
    absDecMax = abs(decMini);
  } else { // if(decMini<0 && decMaxi>0)
    absDecMin = 0;
    if (abs(raMini) < abs(decMaxi)) {
      absDecMax = abs(decMaxi);
    } else {
      absDecMax = abs(decMini);
    }
  }

  long num = ceil((absDecMax - absDecMin) / searchRadius);
  raRadiusIndex = (float *) malloc(num * sizeof (long));
  long i = 0;
  float tmpDec = 0.0;
  for (i = 0; i < num; i++) {
    tmpDec = absDecMin + i*searchRadius;
    raRadiusIndex[i] = getAngleFromGreatCircle(tmpDec, searchRadius);
  }

#ifdef PRINT_CM_DETAIL
    printf("ra radius index length: %d\n", num);
#endif
}

void PartitionSphere::getAreaBoundary(CMStar *head) {

  CMStar *listHead = head;
  CMStar *tmp = listHead;
  float raMin;
  float decMin;
  float raMax;
  float decMax;
  if (tmp != NULL) {
    raMin = tmp->alpha;
    raMax = tmp->alpha;
    decMin = tmp->delta;
    decMax = tmp->delta;
    tmp = tmp->next;
  }
  while (tmp) {
    if (tmp->alpha > raMax) {
      raMax = tmp->alpha;
    } else if (tmp->alpha < raMin) {
      raMin = tmp->alpha;
    }
    if (tmp->delta > decMax) {
      decMax = tmp->delta;
    } else if (tmp->delta < decMin) {
      decMin = tmp->delta;
    }
    tmp = tmp->next;
  }

  raMinf = raMin;
  decMinf = decMin;
  raMaxf = raMax;
  decMaxf = decMax;

  decMini = floor(decMin - areaBox);
  decMaxi = ceil(decMax + areaBox);

  float maxDec = fabs(decMaxi) > fabs(decMini) ? fabs(decMaxi) : fabs(decMini);
  float areaBoxForRa = getAngleFromGreatCircle(maxDec, areaBox);
  raMini = floor(raMin - areaBoxForRa);
  raMaxi = ceil(raMax + areaBoxForRa);
}

void PartitionSphere::getZoneLength() {

  long totalNode = (INDEX_SIZE) / sizeof (CMZone);
  float zoneLengthf = sqrt((decMaxi - decMini + 1)*(raMaxi - raMini + 1)*3600.0 * 3600.0 / totalNode);
  zoneLength = ceil(zoneLengthf);
  if (zoneLength < minZoneLength * 3600)
    zoneLength = ceil(minZoneLength * 3600);
  factor = 3600 / zoneLength;

  decNode = ceil((decMaxi - decMini + 1) * factor);
  raNode = ceil((raMaxi - raMini + 1) * factor);
}

void PartitionSphere::initAreaNode(CMStar *point) {

  getAreaBoundary(point);
  getZoneLength();
  initRaRadiusIndex();

  int totalNode = decNode*raNode;
  zoneArray = (CMZone *) malloc(sizeof (CMZone) * totalNode);

  int i = 0;
  for (i = 0; i < totalNode; i++) {
    zoneArray[i].star = NULL;
    zoneArray[i].starNum = 0;
  }
}

void PartitionSphere::addDataToTree(CMStar *head) {

  long start, end;
  start = clock();

  CMStar *listHead = head;
  CMStar *tmp = listHead->next;
  long branch = 0;
  long i = 0;
  while (tmp) {
    listHead->next = tmp->next; //把tmp点从数据表中移除
    branch = getPointBranch(tmp); //获得tmp所属的树枝位置
    addPointToBranchNotSort(tmp, zoneArray + branch); //把tmp点加入到树干的对应树枝中
    tmp = listHead->next; //取下一个点
    i++;
  }
  branch = getPointBranch(listHead); //获得tmp所属的树枝位置
  addPointToBranchNotSort(listHead, zoneArray + branch); //把tmp点加入到树干的对应树枝中
  i++;
  end = clock();

#ifdef PRINT_CM_DETAIL
    printf("totle point in index: %d\n", i);
    printf("time of init index is: %fs\n", (end - start)*1.0 / ONESECOND);
#endif
}

long *PartitionSphere::getPointSearchBranch(CMStar *point, long *number) {

  int height, width;

  float alpha = point->alpha;
  float delta = point->delta;

  float up = delta + searchRadius; //on north, up > down
  float down = delta - searchRadius; //on south, down > up
  float maxDec = 0.0;
  if (up > 0.0 && down > 0.0) {
    maxDec = up;
  } else if (up < 0.0 && down < 0.0) {
    maxDec = fabs(down);
  } else {
    if (fabs(up) > fabs(down))
      maxDec = fabs(up);
    else
      maxDec = fabs(down);
  }
  /*
      float raRadius = getAngleFromGreatCircle(maxDec, searchRadius);
   */
  int tIndex = ceil((maxDec - absDecMin) / searchRadius);
  float raRadius = raRadiusIndex[tIndex];

  float left = alpha - raRadius;
  float right = alpha + raRadius;

  //-zoneLength/3600
  if (up > 90.0) {
    up = 90.0;
    left = raMinf;
    right = raMaxf;
  } else if (down < -90.0) {
    down = -90.0;
    left = raMinf;
    right = raMaxf;
  }

  int indexUp = (up - decMini) * factor;
  int indexDown = (down - decMini) * factor;
  int indexLeft = (left - raMini) * factor;
  int indexRight = (right - raMini) * factor;

  if (indexUp >= decNode) indexUp = decNode - 1;
  if (indexDown < 0) indexDown = 0;
  if (indexRight >= raNode) indexRight = raNode - 1;
  if (indexLeft < 0) indexLeft = 0;

  height = abs(indexUp - indexDown) + 1;
  width = indexRight - indexLeft + 1;
  *number = height*width;
  long *branch = (long *) malloc(*number * sizeof (long));

  int i, j;
  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      branch[i * width + j] = (indexDown + i) * raNode + indexLeft + j;
    }
  }

  return branch;
}

double PartitionSphere::searchSimilarPoint(CMStar *branch, CMStar *point, CMStar **goalPoint) {

  double error = areaBox;
  int start = 0;
  CMStar *tSample = branch;
  while (tSample) {
    start = 1;
    float distance = getGreatCircleDistance(tSample, point);
    if (distance < error) {
      *goalPoint = tSample;
      error = distance;
    }
    tSample = tSample->next;
  }
  return error;
}

bool PartitionSphere::hasSimilarPoint(CMStar *point) {

  double error = areaBox;

  long numArea = 0;
  long *branchIndex = NULL;
  branchIndex = getPointSearchBranch(point, &numArea);

  for (int i = 0; i < numArea; i++) {
    CMStar *tSample = zoneArray[branchIndex[i]].star;
    while (tSample) {
      float distance = getGreatCircleDistance(tSample, point);
      if (distance < error) {
        return true;
      }
      tSample = tSample->next;
    }
  }
  return false;
}

void PartitionSphere::getMatchStar(CMStar *point) {

  long numArea = 0;
  long *branchIndex = NULL;
  branchIndex = getPointSearchBranch(point, &numArea);

  //232.890152 21.188778
//  if (fabs(point->alpha - 232.890152) < CompareFloat && fabs(point->delta - 21.188778) < CompareFloat) {
//    printf("%s", point->line);
//  }

  double minError = areaBox;
  CMStar *minPoint = NULL;
  for (int i = 0; i < numArea; i++) {
    CMStar *tSample = zoneArray[branchIndex[i]].star;
    while (tSample) {
      float distance = getGreatCircleDistance(tSample, point);
      if (distance < minError) {
        point->matchNum = point->matchNum + 1;
      }
      tSample = tSample->next;
    }
  }

  free(branchIndex);
}

void PartitionSphere::getMatchStar12(CMStar *point) {

  long numArea = 0;
  long *branchIndex = NULL;
  branchIndex = getPointSearchBranch(point, &numArea);

  double minError = areaBox;
  //  double minError = areaBox * 10;
  CMStar *minPoint = NULL;
  for (int i = 0; i < numArea; i++) {
    CMStar *tSample = zoneArray[branchIndex[i]].star;
    while (tSample) {
      float distance = getGreatCircleDistance(tSample, point);
      if (distance < minError) {
        minPoint = tSample;
        minError = distance;
        //        point->matchNum = point->matchNum + 1;
      }
      tSample = tSample->next;
    }
  }
  if (minPoint) {
    point->match = minPoint;
    point->error = minError;
  }

  free(branchIndex);
}

void PartitionSphere::getMatchStar2(CMStar *point) {

  long numArea = 0;
  long *branchIndex = NULL;
  branchIndex = getPointSearchBranch(point, &numArea);

  double minError = areaBox;
  //  double minError = areaBox*10;
  CMStar *minPoint = NULL;
  bool getStar = false;
  for (int i = 0; i < numArea; i++) {
    CMStar *tSample = zoneArray[branchIndex[i]].star;
    while (tSample) {
      float distance = getGreatCircleDistance(tSample, point);
      if (distance < minError) {
        minPoint = tSample;
        minError = distance;
        getStar = true;
        break;
      }
      tSample = tSample->next;
    }
    if (getStar) {
      break;
    }
  }
  if (minPoint) {
    point->match = minPoint;
    point->error = minError;
  }

  free(branchIndex);
}