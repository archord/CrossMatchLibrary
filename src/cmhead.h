/* 
 * File:   cmhead.h
 * Author: xy
 *
 * Created on October 21, 2013, 8:39 AM
 */

#ifndef CMHEAD_H
#define	CMHEAD_H


#define CHECK_IS_NULL(var,varname) \
        {if(var==NULL){\
            sprintf(statusstr, "Error Code: %d\n"\
                "File %s line %d, the input parameter \"%s\" is NULL!\n", \
                GWAC_FUNCTION_INPUT_NULL, __FILE__, __LINE__, varname);\
            return GWAC_FUNCTION_INPUT_NULL;}}
#define CHECK_STRING_NULL_OR_EMPTY(var, varname) \
        {if(var==NULL || strcmp(var, "") == 0){\
            sprintf(statusstr, "Error Code: %d\n"\
                "File %s line %d, string \"%s\" is NULL or empty!\n", \
                GWAC_STRING_NULL_OR_EMPTY, __FILE__, __LINE__, varname);\
            return GWAC_FUNCTION_INPUT_NULL;}}
#define CHECK_OPEN_FILE(fp,fname) \
        {if(fp==NULL){\
            sprintf(statusstr, "Error Code: %d\n"\
                "File %s line %d, open file \"%s\" error!\n", \
                GWAC_OPEN_FILE_ERROR, __FILE__, __LINE__, fname);\
            return GWAC_OPEN_FILE_ERROR;}}

class CMStar {
public:
    long id;
    long starId;
    float pixx;
    float pixy;
    float mag;
    float error;
    char *line;
    CMStar *match;
    CMStar *next;
};

class CMZone {
public:
    int starNum;
    CMStar *star;
};

#define FIND_MOST_LOW
#define PRINT_CM_DETAIL

static const int MaxStringLength = 1024;
static const float CompareFloat = 0.000001;
static const long MaxMallocMemory = 2147483648l; //1GB=1073741824l 2GB=2147483648l

float getLineDistance(CMStar *p1, CMStar *p2);
template<typename T>
inline T getMax(const T& a, const T& b);
long countFileLines(char *fName);

#endif	/* CMHEAD_H */

