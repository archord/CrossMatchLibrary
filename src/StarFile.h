/* 
 * File:   StarFile.h
 * Author: xy
 *
 * Created on December 4, 2014, 9:15 AM
 */

#ifndef STARFILE_H
#define	STARFILE_H

#include <string.h>
#include "cmhead.h"

class StarFile {
public:
  char * fileName;
  CMStar *starList;
  long starNum;
  int mode; //0 赋值到图像坐标系，下一步讲对星表在图像坐标系分区匹配； 1赋值到天球坐标系，下一步讲对星表在天球坐标系分区匹配

public:
  StarFile(int mode=0);
  StarFile(char * fileName, int mode=0);
  StarFile(const StarFile& orig);
  virtual ~StarFile();

  void readStar();
  void readStar(char * fileName, int *idxs = NULL, int pnum = 0);
  void writeStar(char * outFile);

private:
};

#endif	/* STARFILE_H */

