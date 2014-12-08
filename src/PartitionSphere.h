/* 
 * File:   Partition.h
 * Author: xy
 *
 * Created on October 18, 2013, 8:47 AM
 */

#ifndef PARTITIONSPHERE_H
#define	PARTITIONSPHERE_H

#include "cmhead.h"

class PartitionSphere {
protected:

  double errRadius;
  double areaBox; //判断两颗星是一颗星的最大误差，默认为20角秒
  double minZoneLength; //3 times of areaBox
  double searchRadius ; //search radius, great circle

  //for plane coordiante's partition
  int areaWidth;
  int areaHeight;
  float zoneInterval;
  int planeZoneX;
  int planeZoneY;
  int showProcessInfo;
  
  long raMini;
  long decMini;
  long raMaxi;
  long decMaxi;

  float raMinf;
  float decMinf;
  float raMaxf;
  float decMaxf;

  int absDecMin; //in north or south, the max is different,
  int absDecMax;

  int decNode; //number of subarea in dec
  int raNode; //number of subarea in ra
  int zoneLength; //arc second, length of subarea's width or height
  double factor; //=3600/zoneLength

  float *raRadiusIndex;

  CMZone *zoneArray; //分区数组

public:
  PartitionSphere();
  PartitionSphere(const PartitionSphere& orig); 
  PartitionSphere(float errBox, float minZoneLen, float searchRds);
  virtual ~PartitionSphere();

  void initRaRadiusIndex();
  void getAreaBoundary(CMStar *head);
  void getZoneLength();
  void initAreaNode(CMStar *point);
  void addDataToTree(CMStar *head);
  long *getPointSearchBranch(CMStar *point, long *number);
  long getPointBranch(CMStar *point);
  void addPointToBranchSort(CMStar *point, CMZone *branch);
  void addPointToBranchNotSort(CMStar *point, CMZone *branch);
  double searchSimilarPoint(CMStar *branch, CMStar *point, CMStar **goalPoint);
  bool hasSimilarPoint(CMStar *point);
  
  void getMatchStar(CMStar *point);
  void getMatchStar12(CMStar *point);
  void getMatchStar2(CMStar *point);

protected:
};

#endif	/* PARTITION_H */

