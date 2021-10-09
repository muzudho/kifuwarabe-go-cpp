﻿#pragma once

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

/// <summary>
/// n路盤
/// </summary>
const int kBoardSize = 9;

/// <summary>
/// 両端に番兵込みの幅
/// </summary>
const int kWidth = (kBoardSize + 2);

/// <summary>
/// 番兵込みの盤の面積
/// </summary>
const int kBoardMax = (kWidth * kWidth);

/// <summary>
/// x, y を z（座標；配列のインデックス） に変換
/// </summary>
/// <param name="x">is (1 &lt;= x &lt;= 9)</param>
/// <param name="y">is (1 &lt;= y &lt;= 9)</param>
/// <returns></returns>
int GetZ(int x, int y);

/// <summary>
/// for display only
/// </summary>
/// <param name="z">座標</param>
/// <returns>人が読める形の座標</returns>
int Get81(int z);

/// <summary>
/// 同じ命令文で2回呼び出さないでください。
/// don't call twice in same sentence.
///
/// like Prt("z0=%s,z1=%s\n",GetCharZ(z0),GetCharZ(z1));
/// </summary>
/// <param name="z">座標</param>
/// <returns>人が読める形の座標（文字列）</returns>
char *GetCharZ(int z);

/// <summary>
/// プリントなんだけれども 標準エラー出力に出してる？
/// サーバーに出力したくない文字列を表示したいときに使う？
/// </summary>
/// <param name="fmt">書式か？</param>
/// <param name="">可変長引数</param>
void Prt(const char *fmt, ...);

/// <summary>
/// Go Text Protocol のコマンドを標準出力に出力します
/// </summary>
/// <param name="fmt">書式か？</param>
/// <param name="">可変長引数</param>
void SendGtp(const char* fmt, ...);

/// <summary>
/// GTPプロトコルとして送信せずに 引数r を表示？
/// </summary>
/// <param name="r"></param>
void PrtCode64(uint64 r);

/// <summary>
/// 白黒を反転させます
/// </summary>
/// <param name="color"></param>
/// <returns></returns>
int FlipColor(int color);
