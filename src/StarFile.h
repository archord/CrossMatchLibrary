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
  int mode; //0 图像坐标系， 1天球坐标系

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

