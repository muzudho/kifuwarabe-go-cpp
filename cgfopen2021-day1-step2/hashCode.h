#pragma once

#include "common.h"
#include "randXorShift.h"

/// <summary>
/// 1...black, 2...white, 3...ko
/// </summary>
const int kHashKinds = 4;

/// <summary>
/// コウ？
/// </summary>
const int kHashKo = 3;

/// <summary>
/// ハッシュコード
/// </summary>
class HashCode
{
public:
    /// <summary>
    /// ハッシュ コード？
    /// </summary>
    uint64 hashcode = 0;

    /// <summary>
    /// 盤の各交点の、種類別のハッシュ？
    /// </summary>
    ///
    uint64 hashboard[kBoardMax][kHashKinds];

    /// <summary>
    /// 盤の各交点に乱数を割り振っている？
    /// </summary>
    void MakeHashboard();

    /// <summary>
    /// ハッシュコードの 2進数の 0と1 をひっくり返す？
    /// </summary>
    void HashPass();

    /// <summary>
    /// XOR演算する？
    /// </summary>
    /// <param name="z">着手点</param>
    /// <param name="color">手番の色</param>
    void HashXor(int z, int color);
};