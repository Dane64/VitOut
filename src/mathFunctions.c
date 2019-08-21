#include <stdio.h>
#include "mathFunctions.h"

static float rLookupTable[usiRow][usiColumn] =
{
    {0,  	.0000,  1.0000, .0000},
    {1,  	.0175,  .9998, 	.0175},
    {2,  	.0349, 	.9994,  .0349},
    {3,  	.0523,	.9986,  .0524},
    {4,  	.0698,  .9976,  .0699},
    {5,  	.0872,	.9962,  .0875},
    {6,  	.1045,	.9945,  .1051},
    {7,  	.1219,	.9925,  .1228},
    {8,  	.1392,	.9903,  .1405},
    {9,  	.1564,	.9877,  .1584},
    {10,  	.1736,  .9848,  .1763}, 
    {11,  	.1908,  .9816,  .1944},
    {12,  	.2079,  .9781,  .2126},
    {13,  	.2250,  .9744,  .2309},
    {14,  	.2419,  .9703,  .2493},
    {15,  	.2588,  .9659,  .2679},
    {16,  	.2756,  .9613,  .2867},
    {17,  	.2924,  .9563,  .3057},
    {18,  	.3090,  .9511,  .3249},
    {19,  	.3256,  .9455,  .3443},
    {20,  	.3420,  .9397,  .3640},
    {21,  	.3584,  .9336, 	.3839},
    {22,  	.3746,  .9272, 	.4040},
    {23,  	.3907,  .9205, 	.4245},
    {24,  	.4067,  .9135, 	.4452},
    {25,  	.4226,  .9063,	.4663},
    {26,  	.4384,  .8988,	.4877},
    {27,  	.4540,  .8910,	.5095},
    {28,  	.4695,  .8829,  .5317},
    {29,  	.4848,  .8746,  .5543},
    {30,  	.5000,  .8660,  .5774},
    {31,  	.5150,  .8572,  .6009},
    {32,  	.5299,  .8480,  .6249},
    {33,  	.5446,  .8387,  .6494},
    {34,  	.5592,  .8290,  .6745},
    {35,  	.5736,  .8192,  .7002},
    {36,  	.5878,  .8090,  .7265},
    {37,  	.6018,  .7986,  .7536},
    {38,  	.6157,  .7880,  .7813},
    {39,  	.6293,  .7771,  .8098},
    {40,  	.6428,  .7660,  .8391},
    {41,  	.6561,  .7547,  .8693},
    {42,  	.6691,  .7431,  .9004},
    {43,  	.6820,  .7314,  .9325},
    {44,  	.6947,  .7193,  .9657},
    {45,  	.7071, 	.7071, 	1.0000},
};

float sinInterpolate(unsigned char usiDegrees)
{
    float rAngle;
    unsigned char usiFunction = 1;
    unsigned char usiCounter;

    rAngle = rLookupTable[usiDegrees][usiFunction];

    return rAngle;
}

float cosInterpolate(unsigned char usiDegrees)
{
    float rAngle;
    unsigned char usiFunction = 2;
    unsigned char usiCounter;

    rAngle = rLookupTable[usiDegrees][usiFunction];

    return rAngle;
}

float tanInterpolate(unsigned char usiDegrees)
{
    float rAngle;
    unsigned char usiFunction = 3;
    unsigned char usiCounter;

    rAngle = rLookupTable[usiDegrees][usiFunction];

    return rAngle;
}