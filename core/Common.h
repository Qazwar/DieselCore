#pragma once
#include <stdint.h>

typedef uint32_t ID;

typedef uint32_t RID;

const RID INVALID_RID = UINT32_MAX;

const ID INVALID_ID = UINT32_MAX;

const float PI = 3.1415926535897932384626433832795f;
const float TWO_PI = 2.0f * PI;
const float HALF_PI = 0.5f * PI;
const float TWO_PI_INV = 1.0f / TWO_PI;

#define DEGTORAD( degree ) ((degree) * (PI / 180.0f))
#define RADTODEG( radian ) ((radian) * (180.0f / PI))


