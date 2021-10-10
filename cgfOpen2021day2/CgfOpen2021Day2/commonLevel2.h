#pragma once

#include <math.h>
#include <stdarg.h>
#include <stdio.h>

// 64ビット符号なし整数型の定義
#if defined(_MSC_VER)
typedef unsigned __int64 uint64;
#define PRIx64 "I64x"
#else
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
typedef uint64_t uint64; // Linux
#define PRIx64 "llx"
#endif
