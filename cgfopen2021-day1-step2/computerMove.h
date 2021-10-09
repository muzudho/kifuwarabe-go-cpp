#pragma once

#include <string.h>
#include "randXorShift.h"
#include "position.h"
#include "upperConfidenceTree.h"

extern TimeMan timeMan;

/// <summary>
/// コンピューターの指し手
/// </summary>
/// <param name="color">手番の色</param>
/// <param name="search">探索方法。SEARCH_PRIMITIVE または kSearchUct</param>
/// <returns>座標</returns>
int PlayComputerMove(Position position, UpperConfidenceTree uct, int color, int search);

/// <summary>
/// 一番良く打たれた一手の座標を返します。
/// 手番で１回だけ呼び出されます
/// </summary>
/// <param name="color">手番の色</param>
/// <returns>一番良く打たれた一手の座標</returns>
int GetBestUct(Position position, UpperConfidenceTree uct, int color);
