#pragma once

#include "position.h"

class Liberty {
public:

    /// <summary>
    /// count_liberty関数の中で呼び出されます。再帰
    /// </summary>
    /// <param name="tz">着手（開始）座標</param>
    /// <param name="color">連の色</param>
    /// <param name="p_liberty">呼吸点の数</param>
    /// <param name="p_stone">連の石の数</param>
    void CountLibertySub(int tz, int color, int* p_liberty, int* p_stone);

    /// <summary>
    /// 呼吸点の数
    /// </summary>
    /// <param name="tz">着手座標</param>
    /// <param name="p_liberty">呼吸点の数</param>
    /// <param name="p_stone">連の石の数</param>
    void CountLiberty(int tz, int* p_liberty, int* p_stone);
};