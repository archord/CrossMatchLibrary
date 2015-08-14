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

void PartitionSphere::partitonStarField(StarFile *starFile) {

  totalStar = starFile->starNum;
  CMStar *starList = starFile->starList;
  initAreaNode(starList);
  addDataToTree(starList);
}

void PartitionSphere::initAreaNode(CMStar *point) {

  getAreaBoundary(point);
  getZoneLength();
  initRaRadiusIndex();

  totalZone = decNode*raNode;
  zoneArray = (CMZone *) malloc(sizeof (CMZone) * totalZone);

  int i = 0;
  for (i = 0; i < totalZone; i++) {
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

/**
 * 计算误差半径内所有匹配新的个数
 * @param point
 */
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

/**
 * 在误差半径范围内，搜寻所有星，返回距离最小的匹配星
 * @param point
 */
void PartitionSphere::getMatchStar1(CMStar *point) {

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

/**
 * 在误差半径内，只要找到匹配的星，就退出搜寻。
 * @param point
 */
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

/**
 * 在误差半径范围内，记录所有匹配的星
 * @param point
 */
void PartitionSphere::getMatchStar3(CMStar *point) {

  long numArea = 0;
  long *branchIndex = NULL;
  branchIndex = getPointSearchBranch(point, &numArea);

  double minError = areaBox;
  CMStar *tmch = NULL;
  for (int i = 0; i < numArea; i++) {
    if (branchIndex[i] > totalZone - 1) {
      continue;
    }
    CMStar *tSample = zoneArray[branchIndex[i]].star;
    int k = 0;
    while (tSample) {
      float distance = getGreatCircleDistance(tSample, point);
      if (distance < minError) {
        CMStar *tmp = tSample->copy();
        tmp->error = distance;
        point->matchNum = point->matchNum + 1;
        if (NULL == point->match) {
          point->match = tmp;
        } else {
          tmch->next = tmp;
        }
        tmch = tmp;
      }
      tSample = tSample->next;
    }
  }

  free(branchIndex);
}

void PartitionSphere::freeZoneArray() {

  if (NULL != raRadiusIndex) {
    free(raRadiusIndex);
  }

  if (NULL != zoneArray) {
    for (int i = 0; i < totalZone; i++) {
      freeStarList(zoneArray[i].star);
    }
    free(zoneArray);
  }
}

void PartitionSphere::freeStarList(CMStar *starList) {

  if (NULL != starList) {
    CMStar *tStar = starList->next;
    while (tStar) {
      starList->next = tStar->next;
      free(tStar);
      tStar = starList->next;
    }
    free(starList);
  }
}

void PartitionSphere::printPartitionDetail() {

  printf("min ra: %f\n", raMinf);
  printf("min dec: %f\n", decMinf);
  printf("max ra: %f\n", raMaxf);
  printf("max dec: %f\n", decMaxf);
  printf("zone length: %f\n", zoneInterval);
  printf("zone width(ra): %d\n", raNode);
  printf("zone height(dec): %d\n", decNode);
  printf("total zones: %d\n", totalZone);
  printf("total stars: %d\n", totalStar);
}

void PartitionSphere::writePartitionDetail(char *fname) {

  FILE *fp;
  if ((fp = fopen(fname, "w")) == NULL) {
    printf("open file error!!\n");
    return;
  }

  fprintf(fp, "min ra: %f\n", raMinf);
  fprintf(fp, "min dec: %f\n", decMinf);
  fprintf(fp, "max ra: %f\n", raMaxf);
  fprintf(fp, "max dec: %f\n", decMaxf);
  fprintf(fp, "zone length: %f\n", zoneInterval);
  fprintf(fp, "zone width(ra): %d\n", raNode);
  fprintf(fp, "zone height(dec): %d\n", decNode);
  fprintf(fp, "total zones: %d\n", totalZone);
  fprintf(fp, "total stars: %d\n", totalStar);

  fprintf(fp, "\nZone details:\n");
  fprintf(fp, "\nzoneId\txId\tyId\tstarNum\tX...\n");

  int i = 0;
  int j = 0;
  for (i = 0; i < totalZone; i++) {

    if (zoneArray[i].starNum > 0) {
      j++;
      fprintf(fp, "%8d%5d%5d%8d", i + 1, i % raNode, i / raNode, zoneArray[i].starNum);
      CMStar *tmp = zoneArray[i].star;
      while (tmp) {
        fprintf(fp, "%15.8f", tmp->alpha);
        tmp = tmp->next;
      }
      fprintf(fp, "\n");
    }
  }
  fclose(fp);
}