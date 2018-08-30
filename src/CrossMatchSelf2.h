/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CrossMatchSelf.h
 * Author: xy
 *
 * Created on August 17, 2018, 4:38 PM
 */

#ifndef CROSSMATCHSELF2_H
#define CROSSMATCHSELF2_H

#include "Partition.h"
#include "CrossMatch.h"

//#define PRINT_CM_DETAIL


class CrossMatchSelf2 : public CrossMatch {

public:
  void printMatched(char *outfName, CMStar *tstars, float errorBox);
  void printNotMatched(char *outfName, CMStar *tstars);
  void printMatchedDs9(char *outfName, CMStar *tstars, float errorBox);
  void printNotMatchedDs9(char *outfName, CMStar *tstars);
};

#endif /* CROSSMATCHSELF2_H */

