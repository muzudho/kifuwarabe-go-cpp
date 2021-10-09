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
/// 目潰しをエラーとするなら
/// </summary>
const int kFillEyeErr = 1;

/// <summary>
/// 目潰しを合法手とするなら（囲碁のルールでは合法手）
/// </summary>
const int kFillEyeOk = 0;

/// <summary>
/// 石の色を反転
/// </summary>
/// <param name="col">石の色</param>
/// <returns>反転した石の色</returns>
int FlipColor(int col);
