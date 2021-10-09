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
