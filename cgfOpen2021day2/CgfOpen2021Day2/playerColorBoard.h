#pragma once

#include "common.h"

/// <summary>
/// チェック用の盤。プレイヤーの色を埋める
/// </summary>
class PlayerColorBoard {
public:
    /// <summary>
    /// 
    /// </summary>
    int played_color[kBoardMax] = {0};

    /// <summary>
    /// 空点か？
    /// </summary>
    /// <param name="z">座標</param>
    /// <returns></returns>
    bool IsEmpty(int z);

    void SetColor(int z, int color);
};