#pragma once
#include <Windows.h>
#include "..\log\Log.h"

#ifndef XASSERT
#define XASSERT(Expr, s, ...) do { MyAssert_fmt(#Expr, Expr,__FILE__,__LINE__,s,__VA_ARGS__); } while(false);
#endif

