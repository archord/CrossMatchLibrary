/* 
 * File:   cmhead.h
 * Author: xy
 *
 * Created on October 21, 2013, 8:39 AM
 */

#ifndef CMHEAD_H
#define	CMHEAD_H


typedef struct CM_STAR {
    int id;
    float alpha;
    float delta;
    float background;
    float classstar;
    float ellipticity;
    float flags;
    float mag;
    float mage;
    float magnorm;
    float fwhm;
    float magcalib;
    float magcalibe;
    float pixx;
    float pixy;
    float thetaimage;
    float vignet;
    float flux;
    float threshold;
    float error;
    struct CM_STAR *match;
    struct CM_STAR *standrad;
    struct CM_STAR *next;
} cm_star;

typedef struct CM_ZONE {
    int starNum;
    cm_star *star;
} cm_zone;

#define FIND_MOST_LOW
#define PRINT_CM_DETAIL1

float getLineDistance(cm_star *p1, cm_star *p2);

#endif	/* CMHEAD_H */

