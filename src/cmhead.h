/* 
 * File:   cmhead.h
 * Author: xy
 *
 * Created on October 21, 2013, 8:39 AM
 */

#ifndef CMHEAD_H
#define	CMHEAD_H

class CMStar {
public:
    long id;
    long starId;
    float pixx;
    float pixy;
    float mag;
    float error;
    CMStar *match;
    CMStar *next;
};

class CMZone {
public:
    int starNum;
    CMStar *star;
} CMZone;

#define FIND_MOST_LOW
#define PRINT_CM_DETAIL1

static const int MaxStringLength = 1024;
static const float CompareFloat = 0.000001;
static const long MaxMallocMemory = 2147483648l; //1GB=1073741824l 2GB=2147483648l

float getLineDistance(CMStar *p1, CMStar *p2);
template<typename T>
inline T getMax(const T& a, const T& b);
long countFileLines(char *fName);

#endif	/* CMHEAD_H */

