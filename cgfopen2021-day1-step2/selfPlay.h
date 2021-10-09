#pragma once

#include "go15.h"
#include "position.h"
#include "computerMove.h"
#include "upperConfidenceTree.h"

/// <summary>
/// 黒番は原始モンテカルロ、白番はUCTで自己対戦
/// </summary>
void Selfplay(Position position, UpperConfidenceTree uct);
