#pragma once

#include "timeMan.h"

/// <summary>
/// UCTで使われる経路（パス）の最大長
/// </summary>
const int kDMax = 1000;

/// <summary>
/// 原始モンテカルロ探索
/// </summary>
const int kSearchPrimitive = 0;

/// <summary>
/// UCT探索
/// </summary>
const int kSearchUct = 1;

/// <summary>
/// 石の色を反転
/// </summary>
/// <param name="col">石の色</param>
/// <returns>反転した石の色</returns>
int FlipColor(int col);
